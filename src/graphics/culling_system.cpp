#include "core/lux.h"
#include "culling_system.h"

#include "core/array.h"
#include "core/sphere.h"

namespace Lux
{
	
	struct CullingSystemImpl
	{
		Array<Sphere> spheres;

	};

	CullingSystem::~CullingSystem()
	{
		ASSERT(NULL == m_impl);
	}


	void CullingSystem::create()
	{
		ASSERT(NULL == m_impl);
		m_impl = LUX_NEW(CullingSystemImpl);
	}


	void CullingSystem::destroy()
	{
		LUX_DELETE(m_impl);
		m_impl = NULL;
	}

	void CullingSystem::insert(const Array<Sphere>& spheres)
	{
		for (int i = 0; i < spheres.size(); i++)
		{
			m_impl->spheres.push(spheres[i]);
		}
	}


}