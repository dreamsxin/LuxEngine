#pragma once

#include "core/plane.h"

namespace Lux
{
	struct LUX_CORE_API Frustum
	{
		void compute(const Vec3& position, const Vec3& direction, const Vec3& up, float fov, float ratio, float near, float far)
		{
			//Vec3 dir, nc, fc;

			// compute width and height of the near and far plane sections
			float tang = (float)tan(ANG2RAD * fov * 0.5);
			float nh = near * tang;
			float nw = nh * ratio;
			float fh = far  * tang;
			float fw = fh * ratio;

			Vec3 Z = position - direction;
			Z.normalize();

			Vec3 X = crossProduct(up, Z);
			X.normalize();

			Vec3 Y = crossProduct(Z, X);

			Vec3 nc = p - Z * near;
			Vec3 fc = p - Z * far;

			m_plane[(uint32_t)Sides::NEAR].set(-Z, nc);
			m_plane[(uint32_t)Sides::FAR].set(Z, fc);

			Vec3 aux = (nc + Y * nh) - position;
			aux.normalize();
			Vec3 normal = crossProduct(aux, X);
			m_plane[(uint32_t)Sides::TOP].set(normal, nc + Y*nh);

			aux = (nc - Y*nh) - position;
			aux.normalize();
			normal = crossProduct(X, aux);
			m_plane[(uint32_t)Sides::BOTTOM].set(normal, nc - Y*nh);

			aux = (nc - X * nw) - position;
			aux.normalize();
			normal = crossProduct(aux, Y);
			m_plane[(uint32_t)Sides::LEFT].set(normal, nc - X*nw);

			aux = (nc + X * nw) - position;
			aux.normalize();
			normal = crossProduct(Y, aux);
			m_plane[(uint32_t)Sides::RIGHT].set(normal, nc + X*nw);
		}

		enum class Sides : uint32_t	{ NEAR, FAR, LEFT, RIGHT, TOP, BOTTOM, COUNT };
		Plane m_plane[(uint32_t)Sides::COUNT];
	};
}