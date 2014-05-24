#include "core/lux.h"
#include "core/resource_manager_base.h"

#include "core/crc32.h"
#include "core/path.h"
#include "core/path_utils.h"
#include "core/resource.h"
#include "core/resource_manager.h"

namespace Lux
{
	void ResourceManagerBase::create(uint32_t id, ResourceManager& owner)
	{
		owner.add(id, this);
		m_owner = &owner;
	}

	void ResourceManagerBase::destroy(void)
	{ }

	Resource* ResourceManagerBase::get(const Path& path)
	{
		ResourceTable::iterator it = m_resources.find(path);

		if(m_resources.end() != it)
		{
			return *it;
		}

		return NULL;
	}

	Resource* ResourceManagerBase::load(const Path& path)
	{
		Resource* resource = get(path);

		if(NULL == resource)
		{
			resource = createResource(path);
			m_resources.insert(path, resource);
		}
		
		if(resource->isEmpty())
		{
			resource->onLoading();
			resource->doLoad();
		}

		resource->addRef();
		return resource;
	}

	void ResourceManagerBase::load(Resource& resource)
	{
		if(resource.isEmpty())
		{
			resource.onLoading();
			resource.doLoad();
		}

		resource.addRef();
	}

	void ResourceManagerBase::unload(const Path& path)
	{
		Resource* resource = get(path);
		if(NULL != resource)
		{
			unload(*resource);
		}
	}

	void ResourceManagerBase::unload(Resource& resource)
	{
		if(0 == resource.remRef())
		{
			resource.incrementDepCount();
			resource.onUnloading();
			resource.doUnload();
		}
	}

	void ResourceManagerBase::forceUnload(const Path& path)
	{
		Resource* resource = get(path);
		if(NULL != resource)
		{
			forceUnload(*resource);
		}
	}

	void ResourceManagerBase::forceUnload(Resource& resource)
	{
		resource.incrementDepCount();
		resource.onUnloading();
		resource.doUnload();
		resource.m_ref_count = 0;
	}

	void ResourceManagerBase::reload(const Path& path)
	{
		Resource* resource = get(path);
		if(NULL != resource)
		{
			reload(*resource);
		}
	}

	void ResourceManagerBase::reload(Resource& resource)
	{
		if (resource.isReady() || resource.isFailure() || resource.isEmpty())
		{
			resource.onReloading();
			resource.doUnload();
			resource.onLoading();
			resource.doLoad();
		}
	}

	void ResourceManagerBase::releaseAll(void)
	{
		for(ResourceTable::iterator it = m_resources.begin(); m_resources.end() != it; ++it)
		{
			LUX_DELETE(*it);
		}
	}

	ResourceManagerBase::ResourceManagerBase()
		: m_size(0)
	{ }

	ResourceManagerBase::~ResourceManagerBase()
	{ }
}