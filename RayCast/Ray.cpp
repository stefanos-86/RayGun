#include "pch.h"

#include "PI.h"
#include "Ray.h"

namespace rc {
	Ray::Ray(const float x, const float z, const float alpha_rad) :
		x(x),
		z(z),
		alpha_rad(alpha_rad)
	{}

	bool Ray::facing_up() const noexcept
	{
		return (0 <= alpha_rad && alpha_rad < PI);
	}

	bool Ray::facing_right() const noexcept
	{
		// This expression makes me wonder if it would be better to normalize angles between -PI and +PI.
		return (0 <= alpha_rad && alpha_rad < PI / 2) || (3 * PI / 2 <= alpha_rad && alpha_rad < 2 * PI);
	}

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