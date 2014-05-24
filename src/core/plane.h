#pragma once

#include "core/vec3.h"
#include "core/vec4.h"

namespace Lux
{
	struct LUX_CORE_API Plane
	{
		inline Plane()
		{ }

		inline Plane(const Vec3& normal, float d)
			: m_plane(normal.x, normal.y, normal.z, d)
		{ }

		inline Plane(const Vec4& rhs)
			: m_plane(rhs)
		{ }

		inline Plane(const Vec3& p, const Vec3& n)
			: m_plane(n.x, n.y, n.z, -dotProduct(p, n))
		{ }

		void set(const Vec3& normal, float d)
		{
			m_plane.set(normal.x, normal.y, normal.z, d);
		}

		void set(const Vec3& normal, const Vec3& point)
		{
			m_plane.set(normal.x, normal.y, normal.z, -dotProduct(point, normal));
		}

		void set(const Vec4& rhs)
		{
			m_plane.set(rhs);
		}

		Vec4 m_plane;
	};
}
