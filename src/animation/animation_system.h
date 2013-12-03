#pragma once


#include "core/lux.h"
#include "engine/iplugin.h"


namespace Lux
{

	class Engine;
	struct Entity;
	class ISerializer;
	class Universe;

	class LUX_ENGINE_API AnimationSystem : public IPlugin
	{
		public:
			AnimationSystem() { m_impl = 0; }

			virtual bool create(Engine&) LUX_OVERRIDE;
			virtual void update(float time_delta) LUX_OVERRIDE;
			virtual void onCreateUniverse(Universe& universe) LUX_OVERRIDE;
			virtual void onDestroyUniverse(Universe& universe) LUX_OVERRIDE;
			virtual void serialize(ISerializer& serializer) LUX_OVERRIDE;
			virtual void deserialize(ISerializer& serializer) LUX_OVERRIDE;
			virtual Component createComponent(uint32_t, const Entity&) LUX_OVERRIDE;

			void destroy();
			Component createAnimable(const Entity& entity);
			void playAnimation(const Component& cmp, const char* path);
			void setAnimationTime(const Component& cmp, float time);
		

		private:
			struct AnimationSystemImpl* m_impl;
	};


}// ~ namespace Lux 