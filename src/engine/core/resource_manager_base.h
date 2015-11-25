#pragma once


#include "core/pod_hash_map.h"


namespace Lumix
{


namespace FS
{
class FileSystem;
}


class Path;
class Resource;
class ResourceManager;


class LUMIX_ENGINE_API ResourceManagerBase
{
	friend class Resource;
public:
	typedef PODHashMap<uint32, Resource*> ResourceTable;

public:
	void create(uint32 id, ResourceManager& owner);
	void destroy();

	Resource* get(const Path& path);
	Resource* load(const Path& path);
	void add(Resource* resource);
	void remove(Resource* resource);
	void load(Resource& resource);

	void unload(const Path& path);
	void unload(Resource& resource);

	void forceUnload(const Path& path);
	void forceUnload(Resource& resource);

	void reload(const Path& path);
	void reload(Resource& resource);
	ResourceTable& getResourceTable() { return m_resources; }

	ResourceManagerBase(IAllocator& allocator);
	virtual ~ResourceManagerBase();

protected:
	virtual Resource* createResource(const Path& path) = 0;
	virtual void destroyResource(Resource& resource) = 0;

	ResourceManager& getOwner() const { return *m_owner; }
private:
	uint32 m_size;
	ResourceTable m_resources;
	ResourceManager* m_owner;
};


}