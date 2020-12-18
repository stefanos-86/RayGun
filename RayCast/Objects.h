#pragma once

#include <vector>

#include "KdTree.h"
#include "Sprite.h"

namespace rc {
	/** Just a collection to keep track of all the objects that can be seen onscreen. */
	class Objects {
	public:
		Objects() = default;
		Objects(Objects&& x) = default;  /// The KdTree has unique pointer, makes this not copy-able. Thankfully there's never a need to copy. Just move.

		KdTree enemies;  // There may be many enemies, use a "fast" structure for collisions. 
		std::vector<Sprite> exits;  // TODO: do I want to keep multiple exits? Can I cache the cell they are into? Do I need the KdTree here too?

		/** Returns all the hists from the intersection between the ray and any of the sprites.
			Hits further from the ray than the cutoff distance (the distance of the cutoff hit) are discarded.
			Hits are sorted by distance in reverse (the more distant first) to help over-paint them.

			TODO There is also room to improve the memory managment, probably (e. g. do not return a new vector every time).
			TODO: completely untested!		
		*/
		std::vector <RayHit> all_intersections(const Ray& ray, const RayHit& cutoff, const uint8_t enumerated_kinds) const noexcept;

		void deactivate(const uint8_t sprite_id);

		/** Returns the distance to the closest exit - straight line, does not account for walls. */
		float distance_to_closest_exit(const float x, const float z) const noexcept;

	private:
		/// kept for comparison with optimized version. Will be erased, sooner or later. Or kept for simpler collections that do not need trees.
		std::vector <RayHit> intersections(const Ray& ray, const RayHit& cutoff, const std::vector<Sprite>& objects) const;
		std::vector <RayHit> intersections(const Ray& ray, const RayHit& cutoff, const std::vector<uint8_t> broad_phase_indexes) const;
	};
}

