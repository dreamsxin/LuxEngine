#pragma once

//// remove
#include "core/array.h"
#include "core/sphere.h"
//// remove

namespace Lux
{
	class LUX_ENGINE_API CullingSystem
	{
	public:
		CullingSystem() : m_impl(NULL) { }
		~CullingSystem();

		void create();
		void destroy();

		//// test
		void insert(const Array<Sphere>& spheres);
		//// test

	private:
		struct CullingSystemImpl* m_impl;
	};
} // ~namespace Lux