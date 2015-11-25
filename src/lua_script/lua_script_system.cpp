#include "lua_script_system.h"
#include "core/array.h"
#include "core/base_proxy_allocator.h"
#include "core/binary_array.h"
#include "core/blob.h"
#include "core/crc32.h"
#include "core/fs/file_system.h"
#include "core/fs/ifile.h"
#include "core/iallocator.h"
#include "core/json_serializer.h"
#include "core/log.h"
#include "core/lua_wrapper.h"
#include "core/resource_manager.h"
#include "debug/debug.h"
#include "engine.h"
#include "iplugin.h"
#include "plugin_manager.h"
#include "lua_script/lua_script_manager.h"
#include "universe/universe.h"


namespace Lumix
{


static const uint32 SCRIPT_HASH = Lumix::crc32("script");


class LuaScriptSystemImpl;


void registerEngineLuaAPI(Engine&, lua_State* L);
void registerUniverse(UniverseContext*, lua_State* L);
void registerPhysicsLuaAPI(Engine&, Universe&, lua_State* L);
void registerAudioLuaAPI(Engine&, Universe&, lua_State* L);



static const uint32 LUA_SCRIPT_HASH = crc32("lua_script");


class LuaScriptSystem : public IPlugin
{
public:
	LuaScriptSystem(Engine& engine);
	virtual ~LuaScriptSystem();

	IAllocator& getAllocator();
	IScene* createScene(UniverseContext& universe) override;
	void destroyScene(IScene* scene) override;
	bool create() override;
	void destroy() override;
	const char* getName() const override;
	LuaScriptManager& getScriptManager() { return m_script_manager; }

	Engine& m_engine;
	Debug::Allocator m_allocator;
	LuaScriptManager m_script_manager;
};


class LuaScriptSceneImpl : public LuaScriptScene
{
public:
	struct Script
	{
		Script(IAllocator& allocator)
			: m_properties(allocator)
		{
			m_script = nullptr;
		}

		LuaScript* m_script;
		int m_entity;
		lua_State* m_state;
		int m_environment;
		Array<Property> m_properties;
	};


public:
	LuaScriptSceneImpl(LuaScriptSystem& system, UniverseContext& ctx)
		: m_system(system)
		, m_universe_context(ctx)
		, m_scripts(system.getAllocator())
		, m_valid(system.getAllocator())
		, m_global_state(nullptr)
		, m_updates(system.getAllocator())
	{
	}


	~LuaScriptSceneImpl()
	{
		unloadAllScripts();
	}


	void unloadAllScripts()
	{
		for (int i = 0; i < m_scripts.size(); ++i)
		{
			Script& script = m_scripts[i];
			if (m_valid[i] && script.m_script)
			{
				m_system.getScriptManager().unload(*script.m_script);
			}
		}
		m_scripts.clear();
	}


	Universe& getUniverse() override { return *m_universe_context.m_universe; }


	void registerAPI(lua_State* L)
	{
		registerUniverse(&m_universe_context, L);
		registerEngineLuaAPI(m_system.m_engine, L);
		if (m_system.m_engine.getPluginManager().getPlugin("physics"))
		{
			registerPhysicsLuaAPI(m_system.m_engine, *m_universe_context.m_universe, L);
		}
		if (m_system.m_engine.getPluginManager().getPlugin("audio"))
		{
			registerAudioLuaAPI(m_system.m_engine, *m_universe_context.m_universe, L);
		}
	}


	void applyProperty(Script& script, Property& prop)
	{
		if (prop.m_value.length() == 0) return;

		lua_State* state = script.m_state;
		const char* name = script.m_script->getPropertyName(prop.m_name_hash);
		if (!name)
		{
			return;
		}
		char tmp[1024];
		copyString(tmp, name);
		catString(tmp, " = ");
		catString(tmp, prop.m_value.c_str());

		bool errors =
			luaL_loadbuffer(state, tmp, stringLength(tmp), nullptr) != LUA_OK;

		lua_rawgeti(script.m_state, LUA_REGISTRYINDEX, script.m_environment);
		lua_setupvalue(script.m_state, -2, 1);

		errors = errors || lua_pcall(state, 0, LUA_MULTRET, 0) != LUA_OK;

		if (errors)
		{
			g_log_error.log("lua") << script.m_script->getPath().c_str() << ": "
								   << lua_tostring(state, -1);
			lua_pop(state, 1);
		}
	}


	LuaScript* getScriptResource(ComponentIndex cmp) const override
	{
		return getScript(cmp).m_script;
	}


	const char* getPropertyValue(Lumix::ComponentIndex cmp, int index) const override
	{
		auto& script = getScript(cmp);
		uint32 hash = crc32(getPropertyName(cmp, index));

		for (auto& value : script.m_properties)
		{
			if (value.m_name_hash == hash)
			{
				return value.m_value.c_str();
			}
		}

		return "";
	}


	void setPropertyValue(Lumix::ComponentIndex cmp,
		const char* name,
		const char* value) override
	{
		Property& prop = getScriptProperty(cmp, name);
		prop.m_value = value;

		if (m_scripts[cmp].m_state)
		{
			applyProperty(m_scripts[cmp], prop);
		}
	}


	const char* getPropertyName(Lumix::ComponentIndex cmp, int index) const override
	{
		auto& script = getScript(cmp);

		return script.m_script ? script.m_script->getProperties()[index].name : "";
	}


	int getPropertyCount(Lumix::ComponentIndex cmp) const override
	{
		auto& script = getScript(cmp);

		return script.m_script ? script.m_script->getProperties().size() : 0;
	}


	void applyProperties(Script& script)
	{
		if (!script.m_script) return;

		for (Property& prop : script.m_properties)
		{
			applyProperty(script, prop);
		}
	}


	static void* luaAllocator(void* ud, void* ptr, size_t osize, size_t nsize)
	{
		auto& allocator = *static_cast<IAllocator*>(ud);
		if (nsize == 0)
		{
			allocator.deallocate(ptr);
			return nullptr;
		}
		if (nsize > 0 && ptr == nullptr) return allocator.allocate(nsize);

		void* new_mem = allocator.allocate(nsize);
		copyMemory(new_mem, ptr, Math::minValue(osize, nsize));
		allocator.deallocate(ptr);
		return new_mem;
	}


	void startGame() override
	{
		m_global_state = lua_newstate(luaAllocator, &m_system.getAllocator());
		luaL_openlibs(m_global_state);
		registerAPI(m_global_state);
		for (int i = 0; i < m_scripts.size(); ++i)
		{
			if (!m_valid[i] || !m_scripts[i].m_script) continue;

			Script& script = m_scripts[i];

			if (!script.m_script->isReady())
			{
				script.m_state = nullptr;
				g_log_error.log("lua script") << "Script " << script.m_script->getPath().c_str()
											  << " is not loaded";
				continue;
			}

			script.m_state = lua_newthread(m_global_state);
			lua_newtable(script.m_state);
			// reference environment
			lua_pushvalue(script.m_state, -1);
			script.m_environment = luaL_ref(script.m_state, LUA_REGISTRYINDEX);

			// environment's metatable & __index
			lua_pushvalue(script.m_state, -1);
			lua_setmetatable(script.m_state, -2);
			lua_pushglobaltable(script.m_state);
			lua_setfield(script.m_state, -2, "__index");

			// set this
			lua_pushinteger(script.m_state, script.m_entity);
			lua_setfield(script.m_state, -2, "this");

			applyProperties(script);

			bool errors = luaL_loadbuffer(script.m_state,
							  script.m_script->getSourceCode(),
							  stringLength(script.m_script->getSourceCode()),
							  script.m_script->getPath().c_str()) != LUA_OK;
			
			lua_pushvalue(script.m_state, -2);
			lua_setupvalue(script.m_state, -2, 1); // function's environment

			errors = errors || lua_pcall(script.m_state, 0, LUA_MULTRET, 0) != LUA_OK;
			if (errors)
			{
				g_log_error.log("lua") << script.m_script->getPath().c_str() << ": "
									   << lua_tostring(script.m_state, -1);
				lua_pop(script.m_state, 1);
			}
			lua_pop(script.m_state, 1);

			lua_rawgeti(script.m_state, LUA_REGISTRYINDEX, script.m_environment);
			if (lua_getfield(script.m_state, -1, "update") == LUA_TFUNCTION)
			{
				m_updates.push(i);
			}
			lua_pop(script.m_state, 1);
		}
	}


	void stopGame() override
	{
		m_updates.clear();
		for (Script& script : m_scripts)
		{
			script.m_state = nullptr;
		}

		lua_close(m_global_state);
		m_global_state = nullptr;
	}


	ComponentIndex createComponent(uint32 type, Entity entity) override
	{
		if (type == LUA_SCRIPT_HASH)
		{
			LuaScriptSceneImpl::Script& script = m_scripts.emplace(m_system.getAllocator());
			script.m_entity = entity;
			script.m_script = nullptr;
			script.m_state = nullptr;
			m_valid.push(true);
			m_universe_context.m_universe->addComponent(
				entity, type, this, m_scripts.size() - 1);
			return m_scripts.size() - 1;
		}
		return INVALID_COMPONENT;
	}


	void destroyComponent(ComponentIndex component,
								  uint32 type) override
	{
		if (type == LUA_SCRIPT_HASH)
		{
			m_updates.eraseItem(component);
			m_universe_context.m_universe->destroyComponent(
				Entity(m_scripts[component].m_entity), type, this, component);
			m_valid[component] = false;
		}
	}


	void serialize(OutputBlob& serializer) override
	{
		serializer.write(m_scripts.size());
		for (int i = 0; i < m_scripts.size(); ++i)
		{
			serializer.write(m_scripts[i].m_entity);
			serializer.writeString(
				m_scripts[i].m_script ? m_scripts[i].m_script->getPath().c_str()
									  : "");
			serializer.write((bool)m_valid[i]);
			if (m_valid[i])
			{
				serializer.write(m_scripts[i].m_properties.size());
				for (Property& prop : m_scripts[i].m_properties)
				{
					serializer.write(prop.m_name_hash);
					serializer.writeString(prop.m_value.c_str());
				}
			}
		}
	}


	void deserialize(InputBlob& serializer, int) override
	{
		int len = serializer.read<int>();
		unloadAllScripts();
		m_scripts.reserve(len);
		m_valid.resize(len);
		for (int i = 0; i < len; ++i)
		{
			Script& script = m_scripts.emplace(m_system.getAllocator());
			serializer.read(m_scripts[i].m_entity);
			char tmp[MAX_PATH_LENGTH];
			serializer.readString(tmp, MAX_PATH_LENGTH);
			m_valid[i] = serializer.read<bool>();
			script.m_script = static_cast<LuaScript*>(
				m_system.getScriptManager().load(Lumix::Path(tmp)));
			script.m_state = nullptr;
			if (m_valid[i])
			{
				int prop_count;
				serializer.read(prop_count);
				script.m_properties.reserve(prop_count);
				for (int j = 0; j < prop_count; ++j)
				{
					Property& prop =
						script.m_properties.emplace(m_system.getAllocator());
					serializer.read(prop.m_name_hash);
					char tmp[1024];
					tmp[0] = 0;
					serializer.readString(tmp, sizeof(tmp));
					prop.m_value = tmp;
				}
				m_universe_context.m_universe->addComponent(
					Entity(m_scripts[i].m_entity), LUA_SCRIPT_HASH, this, i);
			}
		}
	}


	IPlugin& getPlugin() const override { return m_system; }


	void update(float time_delta) override
	{
		if (!m_global_state) { return; }

		for (auto i : m_updates)
		{
			auto& script = m_scripts[i];
			lua_rawgeti(script.m_state, LUA_REGISTRYINDEX, script.m_environment);
			lua_getfield(script.m_state, -1, "update");
			lua_pushnumber(script.m_state, time_delta);
			if (lua_pcall(script.m_state, 1, 0, 0) != LUA_OK)
			{
				g_log_error.log("lua") << lua_tostring(m_global_state, -1);
				lua_pop(script.m_state, 1);
			}
			lua_pop(script.m_state, 1);
		}
	}


	bool ownComponentType(uint32 type) const override
	{
		return type == LUA_SCRIPT_HASH;
	}


	const Script& getScript(ComponentIndex cmp) const { return m_scripts[cmp]; }


	Property& getScriptProperty(ComponentIndex cmp, const char* name)
	{
		uint32 name_hash = crc32(name);
		for (auto& prop : m_scripts[cmp].m_properties)
		{
			if (prop.m_name_hash == name_hash)
			{
				return prop;
			}
		}

		m_scripts[cmp].m_properties.emplace(m_system.getAllocator());
		auto& prop = m_scripts[cmp].m_properties.back();
		prop.m_name_hash = name_hash;
		return prop;
	}


	const char* getScriptPath(ComponentIndex cmp) override
	{
		return m_scripts[cmp].m_script
				   ? m_scripts[cmp].m_script->getPath().c_str()
				   : "";
	}


	void setScriptPath(ComponentIndex cmp, const char* path) override
	{
		if (m_scripts[cmp].m_script)
		{
			m_system.getScriptManager().unload(*m_scripts[cmp].m_script);
		}

		m_scripts[cmp].m_script = static_cast<LuaScript*>(
			m_system.getScriptManager().load(Lumix::Path(path)));
	}


private:
	LuaScriptSystem& m_system;

	BinaryArray m_valid;
	Array<Script> m_scripts;
	lua_State* m_global_state;
	UniverseContext& m_universe_context;
	Array<int> m_updates;
};


LuaScriptSystem::LuaScriptSystem(Engine& engine)
	: m_engine(engine)
	, m_allocator(engine.getAllocator())
	, m_script_manager(m_allocator)
{
	m_script_manager.create(crc32("lua_script"), engine.getResourceManager());
}


LuaScriptSystem::~LuaScriptSystem()
{
	m_script_manager.destroy();
}


IAllocator& LuaScriptSystem::getAllocator()
{
	return m_allocator;
}


IScene* LuaScriptSystem::createScene(UniverseContext& ctx)
{
	return LUMIX_NEW(m_allocator, LuaScriptSceneImpl)(*this, ctx);
}


void LuaScriptSystem::destroyScene(IScene* scene)
{
	LUMIX_DELETE(m_allocator, scene);
}


bool LuaScriptSystem::create()
{
	return true;
}


void LuaScriptSystem::destroy()
{
}


const char* LuaScriptSystem::getName() const
{
	return "lua_script";
}


extern "C" LUMIX_LIBRARY_EXPORT IPlugin* createPlugin(Engine& engine)
{
	return LUMIX_NEW(engine.getAllocator(), LuaScriptSystem)(engine);
}


} // ~namespace Lumix
