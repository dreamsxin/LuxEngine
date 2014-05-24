#pragma once

#include "vec4.h"

namespace Lux
{
	struct Sphere
	{
		Sphere(float x, float y, float z, float radius)
			: m_sphere(x, y, z, radius)
		{}

		Sphere(const Vec3& point, float radius)
			: m_sphere(point.x, point.y, point.z, radius)
		{}

		explicit Sphere(const Vec4& sphere)
			: m_sphere(sphere)
		{}

		Vec4 m_sphere;
	};
}