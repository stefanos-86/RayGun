#include "pch.h"
#include "Sprite.h"

#include <algorithm>
#include <cmath>
#include <float.h>
#include <stdexcept>

#include "Canvas.h"
#include "PI.h"

#include <iostream>

namespace rc {
		
Sprite::Sprite(const float x_position, const float z_position, const uint8_t size, const uint8_t id, const TextureIndex kind) :
	x(x_position),
	z(z_position),
	size(size),
	id(id),
	kind(kind),
	active(true)
{
}

/** To understand the formula, try to picture it like this. 
   Z ^
	 |        / ray
	 |   I  \/
	 |      /\ C     -
	 |     /  \      | H
	 |    /    \     | 
	 |   O    sprite -
	 |   |--L---|  
   --|-------------------------------------> X

   The ray start in O at angle alpha with the X axis.
   The sprite position is C.

   We want the intersection I and the distances between I and O and I and C.
   The first we will use to scale the sprite (the farthest, the smaller), the second
   is to find the texture slice to draw.

   There is a drawing in the repo to explain the construction.

   This is an intentionally stupid algorithm. The best way probably is to intersect the
   segments between the ray origin and any wall hit and between the two endpoints of
   the sprite. But, on a dare, I am trying to not use vector math.
*/
RayHit Sprite::intersection(const Ray& ray) const
{
	const float vertical_difference = z - ray.z;  // H in the drawing.
	float horizontal_difference = x - ray.x;  // L in the drawing.

	// Fudge it to avoid divide by 0 and to address the special case.
	// It should be still "close enough". No, I don't think this is the correct way.
	if (horizontal_difference == 0)
		horizontal_difference = FLT_MIN;

	// Call B the point which is "below" C, at the height of O.
	// OCB has a 90� angle in B. Call beta the angle in O: you can find it
	// given the sides of the triangle.
	// OCB and OIB share a side, where the ray alpha (angle in O of IOB) and
	// beta are side by side. Subtract them and find the angle in O of OIC.
	const float beta = std::atan(vertical_difference / horizontal_difference);
	const float gamma = ray.alpha_rad - beta;

	// Imagine that the sprite is always facing the observer.In the OIC triangle, the angle in
	// C is 90�. Resolve the sides with the usual trig.
	const float distance_OC = std::sqrt(horizontal_difference * horizontal_difference + vertical_difference * vertical_difference);
	const float distance_OI = distance_OC / std::cos(gamma);
	float distance_CI = distance_OI * std::sin(gamma);

	const float half_span = size / 2;
	RayHit result;
	result.hit_object_id = id;
	result.type = kind;

	if ((distance_OI * horizontal_difference) < 0 ||  // Behind the ray. Discovered by trial and error.
		(std::abs(distance_CI) > half_span)) // Ouside the segment "covered" by the texture. Half-size each side of the center.
		return result;
	
	// Did actually hit, fill all the data.
	result.distance = std::abs(distance_OI);


	// Texture offsets are non-negative, but the computation uses the sprite center,
	// so re-center the offset to have 0 offset on the side of the texture.
	result.offset = distance_CI + half_span;

	// We will never use the exact coordinate: not set on purpose.
	
	return result;
}


void Objects::build_accelerators() {
	std::vector<const Sprite*> enemies_addresses;
	for (const auto& x : enemies)
		enemies_addresses.push_back(&x); // TODO: betting that the enemies will never move!

	enemies_accelerator = KdTree::build(enemies_addresses, 20, 20);  // TODO! Review completely the tree structure???
}

std::vector<RayHit> Objects::all_intersections(const Ray& ray, const RayHit& cutoff, const uint8_t enumerated_kinds) const noexcept
{
	std::vector<RayHit> valid_hits;  // Do not reserve. There are few interesction at the same time, not worth it.
	
	if (enumerated_kinds & (uint8_t) TextureIndex::ENEMY) {
		// TODO assert(enemies_accelerator)
		//const auto broad_phase_hits = enemies_accelerator->intersect(ray, cutoff.distance);
		//const std::vector<RayHit> hits = intersections(ray, cutoff, broad_phase_hits);
		//std::cout << "BF " << broad_phase_hits.size() << " NF " << hits.size() << " tot " << enemies.size() << "\n";
		const std::vector<RayHit> hits = intersections(ray, cutoff, enemies);
		
		valid_hits.insert(valid_hits.end(), hits.begin(), hits.end());
	}

	if (enumerated_kinds & (uint8_t)TextureIndex::EXIT) {
		const std::vector<RayHit> hits = intersections(ray, cutoff, exits);
		valid_hits.insert(valid_hits.end(), hits.begin(), hits.end());
	}

	std::sort(valid_hits.begin(), valid_hits.end(),
		[](const RayHit& h1, const RayHit& h2) {
			return h2.distance < h1.distance;  // Notice the reverse order!
		}
	);

	return valid_hits;
}

void Objects::deactivate(const uint8_t sprite_id)  // TODO! Mark that this is only for enemies!!!
{
	const auto to_be_shut_off = std::find_if(enemies.begin(), enemies.end(),
		[sprite_id](const Sprite& s) {
			return s.id == sprite_id;
		}
	);

	if (to_be_shut_off == enemies.end())
		throw std::runtime_error("Attempting to deactivate a sprite that is not there.");

	to_be_shut_off->active = false;
}

std::vector<RayHit> Objects::intersections(const Ray& ray, const RayHit& cutoff, std::vector<Sprite> objects) const
{
	std::vector<RayHit> hits;
	for (const Sprite& sprite : objects) {
		if (!sprite.active)  // ALSO todo: can't deactivate landmarks... can I hide the exits?
			continue;

		RayHit candidate_hit = sprite.intersection(ray);

		if (candidate_hit.really_hit() &&
			cutoff.really_hit() &&  // TODO: may be improper. Should I assume that a no hit cut off means "draw nothing"? It should be "draw everything".
			candidate_hit.distance < cutoff.distance)
		{
			hits.push_back(candidate_hit);
		}
	}
	return hits;
}

std::vector<RayHit> Objects::intersections(const Ray& ray, const RayHit& cutoff, std::vector<const Sprite*> objects) const
{
	std::vector<RayHit> hits;
	for (const Sprite* sprite : objects) {
		if (!sprite->active)  // ALSO todo: can't deactivate landmarks... can I hide the exits?
			continue;

		RayHit candidate_hit = sprite->intersection(ray);

		if (candidate_hit.really_hit() &&
			cutoff.really_hit() &&  // TODO: may be improper. Should I assume that a no hit cut off means "draw nothing"? It should be "draw everything".
			candidate_hit.distance < cutoff.distance)
		{
			hits.push_back(candidate_hit);
		}
	}
	return hits;
}



}