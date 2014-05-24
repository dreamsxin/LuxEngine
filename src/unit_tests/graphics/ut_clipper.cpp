#include "unit_tests/suite/lux_unit_tests.h"

#include "core/sphere.h"
#include "graphics/culling_system.h"

namespace
{

	void UT_culling_system(const char* params)
	{
		Lux::Array<Lux::Sphere> spheres;
		for (float i = 0.f; i < 100.f; i += 1.f)
		{
			spheres.push(Lux::Sphere(i, 0.f, 0.f, 5.f));
		}

		Lux::CullingSystem culling_system;

		culling_system.create();
		culling_system.insert(spheres);
		
	}
}

REGISTER_TEST("unit_tests/graphics/culling_system", UT_culling_system, "");
