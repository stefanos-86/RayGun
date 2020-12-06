#include "pch.h"
#include "KdTree.h"

#include <algorithm>
#include <numeric>
#include <limits>

#include "Sprite.h"

namespace rc {
	void KdTree::build(
		const uint8_t max_depth,
		const uint8_t small_enough_size)
	{
		// TODO: test max depth > 0.
		// TODO: fail in case there are more than 255 objects.
		node_content = std::vector<uint8_t>(objects.size(), 0);
		std::iota(node_content.begin(), node_content.end(), 0);
		split(max_depth - 1, small_enough_size, objects);
	}

	std::vector<uint8_t> KdTree::intersect(const Ray& ray, const float cutoff_distance) const
	{ 
		if (low == nullptr && high == nullptr)  // That is, this is a leaf node.
			return node_content;

		const float ray_origin = (partition_direction == Partition::ON_X) ?
			ray.x : ray.z;

		const bool goes_towards_high = (partition_direction == Partition::ON_X) ?
			ray.facing_right() : ray.facing_up();

		const float tangent = (partition_direction == Partition::ON_X) ?
			std::tan(ray.alpha_rad) : 1 / std::tan(ray.alpha_rad);

		std::vector<uint8_t> from_low, from_high;
		Ray ray_other_side(0, 0, 0);
		const float new_cutoff = ray_on_other_side(ray, ray_other_side, cutoff_distance, ray_origin);

		if (ray_origin < split_point) {
			from_low = low->intersect(ray, cutoff_distance);
			if (new_cutoff > 0 && goes_towards_high)
				from_high = high->intersect(ray_other_side, new_cutoff);
		}
		else if (ray_origin > split_point) {
			from_high = high->intersect(ray, cutoff_distance);
			if (new_cutoff > 0 &&  ! goes_towards_high)
				from_low = low->intersect(ray_other_side, new_cutoff);
		}
		else
		{
			from_high = high->intersect(ray, cutoff_distance);
			from_low = low->intersect(ray, cutoff_distance);
		}

		// Merge vectors, remove duplicates. TODO: do I have to do this at every recursive call?
		from_low.insert(from_low.end(), from_high.begin(), from_high.end());
		std::sort(from_low.begin(), from_low.end());
		from_low.erase(std::unique(from_low.begin(), from_low.end()), from_low.end());

		return from_low;
	}

	void KdTree::split(uint8_t depth, const uint8_t small_enough_size, const std::vector<Sprite>& objects_collection)
	{
		
		if (depth == 0)
			return; // Max depth reached, can not split further.

		if (node_content.size() <= small_enough_size)
			return; // Node is small, no need to split.

		// Nope, we have to split.
		partition_direction = largest_extent(split_point, objects_collection);
		// No area euristic, for simplicity. Cut in half and hope for the best.

		// Create both subtrees. I don't think you can end up with an empty node here.
		low = std::make_unique<KdTree>();
		high = std::make_unique<KdTree>();

		split_low_high(partition_direction, split_point, objects_collection, low->node_content, high->node_content);
		node_content.clear(); // Not a leaf, it has not content. It has been split amond subnodes.

		const uint8_t next_level = depth - 1;
		low->split(next_level, small_enough_size, objects_collection);
		high->split(next_level, small_enough_size, objects_collection);
	}

	KdTree::Partition KdTree::largest_extent(float& split_point, const std::vector<Sprite>& objects_collection) const
	{
		float min_x = std::numeric_limits<float>::max();
		float max_x = std::numeric_limits<float>::min();

		float min_z = std::numeric_limits<float>::max();
		float max_z = std::numeric_limits<float>::min();

		for (const uint8_t item_index : node_content) {
			const Sprite& item = objects_collection.at(item_index);

			const float half_span = item.size;

			min_x = std::min(item.x - half_span, min_x);
			min_z = std::min(item.z - half_span, min_z);
			max_x = std::max(item.x + half_span, max_x);
			max_z = std::max(item.z + half_span, max_z);
		}

		const float x_extent = max_x - min_x;
		const float z_extent = max_z - min_z;
		
		if (x_extent > z_extent) {
			split_point = x_extent / 2 + min_x;
			return KdTree::Partition::ON_X;
		}
		else
		{
			split_point = z_extent / 2 + min_z;
			return KdTree::Partition::ON_Z;
		}
	}

	void KdTree::split_low_high(const KdTree::Partition direction, const float split_point, const std::vector<Sprite>& objects_collection, std::vector<uint8_t>& low, std::vector<uint8_t>& high) const
	{
		for (const uint8_t object_index : node_content) {
			const Sprite& object = objects_collection.at(object_index);
			const float half_span = object.size / 2;
			const float position = (direction == Partition::ON_X) ?
				object.x : object.z;

			const bool on_low_side = position - half_span <= split_point;
			const bool on_high_side = position + half_span >= split_point;
			// Yes, an object can be across the split point and so be in both sides.

			if (on_low_side)
				low.push_back(object_index);
			if (on_high_side)
				high.push_back(object_index);
		}
	}
	float KdTree::ray_on_other_side(const Ray& ray_this_side, Ray& ray_other_side, const float original_cutoff_distance, const float ray_origin) const noexcept
	{
	
		// TODO: all the intersections have to deal with a max distance. Make it part of the ray.
		//       The only exception is the wall intersection, but it is just a matter of starting with a very high value...
		//       It also avoids the output parameter for the new cutoff distance here.

		// This way the horyzontal or vertical intersection "look" the same.
		const float tangent = (partition_direction == Partition::ON_X) ?
			std::tan(ray_this_side.alpha_rad) : 1 / std::tan(ray_this_side.alpha_rad);

		const float distance_from_split = std::abs(ray_origin - split_point);
		const float height_over_ray = distance_from_split * tangent;
		const float distance_to_cross_point = std::sqrt(distance_from_split * distance_from_split + height_over_ray * height_over_ray);  // I have this formula all over the place. TODO!

		if (distance_to_cross_point < original_cutoff_distance) {
			const float new_cutoff = original_cutoff_distance - distance_to_cross_point;  // The ray has to walk "the rest of the way".

			const float new_ray_x = (partition_direction == Partition::ON_X) ?
				split_point : ray_this_side.x + height_over_ray;

			const float new_ray_z = (partition_direction == Partition::ON_X) ?
				ray_this_side.z + height_over_ray : split_point;

			ray_other_side.x = new_ray_x;
			ray_other_side.z = new_ray_z;
			ray_other_side.alpha_rad = ray_this_side.alpha_rad;

			return new_cutoff;
		}

		return -1; // To signify "no ray on the other side".
	}

}