#pragma once

#include <cstdint>

/** Structures to describe the generic ray, to support all the ray-casting variants. */

namespace rc{
	/** A ray that can be used for ray casting. Graphically:
   Z
   ^      _
   |      /|
   |     /
   |    /alpha
   |   *----
   |  (x,z)
   |
	---------------> X
*/
	class Ray {
	public:
		Ray(const float x, const float z, const float alpha_rad);
		float x;
		float z;
		float alpha_rad;

		bool facing_up() const noexcept;  // TODO: test.
		bool facing_right() const noexcept;  // TODO: test.

	};


	/** Represent the hit of a ray on an object, carrying the data to the rest of the system.
	It remembers the exact coordinates of the hit, the distance of the intersection
	from the ray origin and the offset along the cell/object side of the hit (useful for texturing).
	*/
	class RayHit {
	public:
		/** Defaults to a ray that did not hit anything.*/
		RayHit();

		/** Returns true if this is the result of a ray cast that did not find
			any intersection. All the variables may be set to random values.
			Once the distance is set, the RayHit becomes valid.*/
		bool no_hit() const noexcept;

		/** Dual of no_hit(). The data describes the ray-object intersection. */
		bool really_hit() const noexcept;

		float x;
		float z;
		float distance;
		uint8_t offset;
		uint8_t hit_object_id; /// May not be always set. TODO: smell...

	private:
		static constexpr float NO_HIT = -1;  // This is an impossible value for the distance.
	};
}