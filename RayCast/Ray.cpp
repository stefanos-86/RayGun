#include "pch.h"
#include "Ray.h"

namespace rc {
	Ray::Ray(const float x, const float z, const float alpha_rad) :
		x(x),
		z(z),
		alpha_rad(alpha_rad)
	{}

	RayHit::RayHit() :
		x(0),
		z(0),
		distance(RayHit::NO_HIT),
		offset(0)
	{}

	bool RayHit::no_hit() const noexcept {
		return distance == NO_HIT;
	}

	bool RayHit::really_hit() const noexcept {
		return !no_hit();
	}
}