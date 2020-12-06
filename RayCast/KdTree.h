#pragma once

#include <memory>
#include <vector>

#include "Ray.h"
#include "Sprite.h"

namespace rc {

	/** "Inner class" for the KdTree. Refer to that class to know what is going on.
	The tree and the nodes are in different classes to avoid multiple copies of fields and to
	do certain operations only at the end of the intersection recursion.
	*/
	class KdTreeNode
	{
	public:
		enum class Partition : uint8_t {
			ON_X, ON_Z
		};

		std::vector<uint8_t> intersect(const Ray& ray, const float cutoff_distance) const;

		/** The line in the x-z plane that splits the low and high regions. */
		float split_value;
		KdTreeNode::Partition partition_direction;
		std::unique_ptr<KdTreeNode> low;
		std::unique_ptr<KdTreeNode> high;

		std::vector<uint8_t> node_content;  // Only the leaves have this.
		//TODO: to micro optimize, add a LEAF value to the partition directions and put [content], [low, high and split point] in a union.
		

		/** For the recursive tree construction. Divides the node content in the sub trees.*/
		void split(uint8_t depth, const uint8_t small_enough_size, const std::vector<Sprite>& object_collection);

	private:
		/** Tells on what direction (x/z) the objects in the node extend the most.
		It also set the split value - it is computed easily as part of the partition direction calculation.
		In this case it is easier to have a function that does 2 things rather than separating the logic. */
		KdTreeNode::Partition largest_extent(float& split_value, const std::vector<Sprite>& objects_collection) const;

		/** Divides the objects in the node above and below the split value.
		It follows the partition direction set in the node itself.*/
		void split_low_high(
			const float split_value,
			const std::vector<Sprite>& objects_collection,
			std::vector<uint8_t>& low,
			std::vector<uint8_t>& high) const;
		
		/** When the ray goes across the split value, this function sets the partial ray that should be used
		to compute the intersection on the other node.
		
		Returns the cutoff distance for the new intersection (the length of the portion of the ray that is
		across the split). 
		*/
		float ray_on_other_side(Ray& ray_other_side, const Ray& ray_this_side, const float original_cutoff_distance) const noexcept;
	};



	/** Pedestrian implementaton of a KD-tree, specialized to speed up the ray-sprite intersection tests.
	
	The code was already fast enough without it, but it is the principle of the thing: ancient FPS games
	used BSPs to speed up rendering, so here is one.

	The simple implemetation seen here already has very good effects when there are many sprites around.
	*/
	class KdTree {
	public:

		/** Creates the tree structure.
		It may not be the fastets implementation, but in this game all the objects are static:
		the tree is build only once, at startup. No need to worry. */
		void build(const uint8_t max_depth, const uint8_t small_enough_size);

		/** Returns the indices of the objects that may have an intersaction with the ray.
		this->objects.at(any of the indices) points to the object itself. */
		std::vector<uint8_t> intersect(const Ray& ray, const float cutoff_distance) const;

		/** Actual storage of the objects in space.
		For simplicity, the root holds the objects. The tree nodes refer to it via an index (position) 
		in the vector. Using the object requires an extra lookup (given the index, find the object),
		but at least we don't have to handle pointers. */
		std::vector<Sprite> objects;

		/** Root of the tree proper. 
		Ideally, it would be private. But I need to test the tree structure, so this has to be accessible.*/
		KdTreeNode root;
	};
}
