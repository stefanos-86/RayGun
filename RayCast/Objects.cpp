#include "pch.h"
#include "Objects.h"

#include <algorithm>
#include <stdexcept>

namespace rc {

	std::vector<RayHit> Objects::all_intersections(const Ray& ray, const RayHit& cutoff, const uint8_t enumerated_kinds) const noexcept
	{
		std::vector<RayHit> valid_hits;  // Do not reserve. There are few interesction at the same time, not worth it.

		if (enumerated_kinds & (uint8_t)TextureIndex::ENEMY) {
			// TODO assert(enemies was built)

			const auto broad_phase_hits = enemies.intersect(ray, cutoff.distance);
			const std::vector<RayHit> hits = intersections(ray, cutoff, broad_phase_hits);
			//std::cout << "BF " << broad_phase_hits.size() << " NF " << hits.size() << "\n";
			
			// Unoptimized code, to compare...
			//const std::vector<RayHit> hits = intersections(ray, cutoff, enemies.objects);

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
		const auto to_be_shut_off = std::find_if(enemies.objects.begin(), enemies.objects.end(),
			[sprite_id](const Sprite& s) {
				return s.id == sprite_id;
			}
		);

		if (to_be_shut_off == enemies.objects.end())
			throw std::runtime_error("Attempting to deactivate a sprite that is not there.");

		to_be_shut_off->active = false;
	}

	std::vector<RayHit> Objects::intersections(const Ray& ray, const RayHit& cutoff, const std::vector<Sprite>& objects) const
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

	std::vector<RayHit> Objects::intersections(const Ray& ray, const RayHit& cutoff, const std::vector<uint8_t> broad_phase_indexes) const
	{
		std::vector<RayHit> hits;
		for (const uint8_t sprite_index : broad_phase_indexes) {
			const Sprite& sprite = enemies.objects.at(sprite_index);

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
}