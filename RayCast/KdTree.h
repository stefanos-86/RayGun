#pragma once

#include <memory>
#include <vector>

#include "Ray.h"
#include "Sprite.h"

namespace rc {

	class KdTree
	{
	public:
		enum class Partition : uint8_t {
			ON_X, ON_Z
		};

		//KdTree() = default;
		//KdTree(KdTree&&) = default;

		void build(const uint8_t max_depth, const uint8_t small_enough_size);

		std::vector<uint8_t> intersect(const Ray& ray, const float cutoff_distance) const;

		std::unique_ptr<KdTree> low;
		std::unique_ptr<KdTree> high;
		std::vector<uint8_t> node_content;  // Only the leaves have this.
		//TODO: to micro optimize, add a LEAF value to the partition directions and put [content/objects], [low, high and split point] in a union.

		KdTree::Partition partition_direction;
		float split_point; //TODO: it's a point on the axis, but it is actually a line

		std::vector<Sprite> objects;  // Only the root has this. Create a "inner node" class?

	private:
		void split(uint8_t depth, const uint8_t small_enough_size, const std::vector<Sprite>& object_collection);


		// The split point computation "just fit" here, it's convenient
		// to compute all in a go.
		KdTree::Partition largest_extent(float& split_point, const std::vector<Sprite>& objects_collection) const;

		void split_low_high(
			const KdTree::Partition direction,
			const float split_point,
			const std::vector<Sprite>& objects_collection,
			std::vector<uint8_t>& low,
			std::vector<uint8_t>& high) const;
		
		float ray_on_other_side(const Ray& ray_this_side, Ray& ray_other_side, const float original_cutoff_distance, const float ray_origin) const noexcept;
	};

}
