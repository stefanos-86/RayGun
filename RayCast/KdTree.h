#pragma once

#include <memory>
#include <vector>

#include "Ray.h"

namespace rc {
	class Sprite;

	class KdTree
	{
	public:
		enum class Partition : uint8_t {
			ON_X, ON_Z
		};

		static std::shared_ptr<KdTree> build(const std::vector<const Sprite*>& objects, 
			const uint8_t max_depth, 
			const uint8_t small_enough_size);

		std::vector<const Sprite*> intersect(const Ray& ray, const float cutoff_distance) const;

		std::unique_ptr<KdTree> low;
		std::unique_ptr<KdTree> high;
		std::vector<const Sprite*> content;  // Does NOT own the sprites.
		//TODO: to micro optimize, add a LEAF value to the partition directions and put [content], [low, high and split point] in a union.

		KdTree::Partition partition_direction;
		float split_point; //TODO: it's a point on the axis, but it is actually a line

	private:
		void split(uint8_t depth, const uint8_t small_enough_size);


		// The split point computation "just fit" here, it's convenient
		// to compute all in a go.
		KdTree::Partition largest_extent(float& split_point) const;

		void split_low_high(const KdTree::Partition direction, const float split_point, std::vector<const Sprite*>& low, std::vector<const Sprite*>& high) const;
		
		float ray_on_other_side(const Ray& ray_this_side, Ray& ray_other_side, const float original_cutoff_distance, const float ray_origin) const noexcept;
	};

}
