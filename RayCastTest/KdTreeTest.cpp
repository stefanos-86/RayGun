#include "pch.h"

#include "KdTree.h"

#include <vector>

#include "PI.h"
#include "Ray.h"
#include "Sprite.h"


#define ASSERT_NULL(p) ASSERT_FALSE(p)
#define ASSERT_NOT_NULL(p) ASSERT_TRUE(p)

namespace rc {
	TEST(kdTree, Build__empty) {
		KdTree tree;
		tree.build(10, 2);

		ASSERT_NULL(tree.low);
		ASSERT_NULL(tree.high);
		ASSERT_TRUE(tree.objects.empty());
	}
	
	TEST(kdTree, Build__one_element) {
		const uint8_t sprite_id = 1;

		KdTree tree;
		tree.objects.emplace_back(0, 0, 64, sprite_id, TextureIndex::ENEMY);
		tree.build(10, 2);

		ASSERT_NULL(tree.low);
		ASSERT_NULL(tree.high);
		ASSERT_EQ(1, tree.node_content.size());
		ASSERT_EQ(sprite_id, tree.objects.at(tree.node_content.front()).id);
	}
	
	TEST(kdTree, Build__two_elements__split) {
		const Sprite low(-100, 0, 64, 0, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 1, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);  // Index implicitly 0.
		tree.objects.emplace_back(high); // Index implicitly 1.

		tree.build(10, 1);  // Each node can have only 1 element.

		ASSERT_EQ(tree.partition_direction, KdTree::Partition::ON_X);
		ASSERT_NOT_NULL(tree.low);
		ASSERT_NOT_NULL(tree.high);
		ASSERT_EQ(1, tree.low->node_content.size());
		ASSERT_EQ(0, tree.low->node_content.front());
		ASSERT_EQ(1, tree.high->node_content.size());
		ASSERT_EQ(1, tree.high->node_content.front());

		// Double check with object indices.
		ASSERT_EQ(0, tree.objects.at(tree.low->node_content.front()).id);
		ASSERT_EQ(1, tree.objects.at(tree.high->node_content.front()).id);
	}
	
	
	TEST(kdTree, Build__element_in_the_middle_goes_both_sides) {
		const Sprite both(0, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite low(-100, 0, 64, 2, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 3, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(both);
		tree.objects.emplace_back(high);

		tree.KdTree::build(10, 2);

		ASSERT_EQ(tree.partition_direction, KdTree::Partition::ON_X);
		ASSERT_NOT_NULL(tree.low);
		ASSERT_NOT_NULL(tree.high);
		ASSERT_EQ(2, tree.low->node_content.size());
		ASSERT_EQ(0, tree.low->node_content.front());
		ASSERT_EQ(1, tree.low->node_content.back());  // Object on both sides.
		ASSERT_EQ(2, tree.high->node_content.size());
		ASSERT_EQ(1, tree.high->node_content.front());  // Object on both sides.
		ASSERT_EQ(2, tree.high->node_content.back());
	}
	

	TEST(kdTree, Build__split_two_levels) {
		const Sprite left_down(-200, -100, 64, 1, TextureIndex::ENEMY);
		const Sprite left_up(-200, 100, 64, 2, TextureIndex::ENEMY);
		const Sprite right_down(200, -100, 64, 3, TextureIndex::ENEMY);
		const Sprite right_up(200, 100, 64, 4, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(left_down);
		tree.objects.emplace_back(left_up);
		tree.objects.emplace_back(right_down);
		tree.objects.emplace_back(right_up);
		
		tree.build(10, 1);

		// Sad jungle of assertions, but has to explore the whole tree.

		ASSERT_EQ(tree.partition_direction, KdTree::Partition::ON_X);
		ASSERT_NOT_NULL(tree.low);
		ASSERT_NOT_NULL(tree.high);
		const KdTree& low_x = *(tree.low);
		const KdTree& high_x = *(tree.high);
		
		ASSERT_EQ(low_x.partition_direction, KdTree::Partition::ON_Z);
		ASSERT_NOT_NULL(low_x.low);
		ASSERT_NOT_NULL(low_x.high);
		ASSERT_EQ(0, low_x.low->node_content.front());
		ASSERT_EQ(1, low_x.high->node_content.front());


		ASSERT_EQ(high_x.partition_direction, KdTree::Partition::ON_Z);
		ASSERT_NOT_NULL(high_x.low);
		ASSERT_NOT_NULL(high_x.high);
		ASSERT_EQ(2, high_x.low->node_content.front());
		ASSERT_EQ(3, high_x.high->node_content.front());
	}
	
	TEST(kdTree, intersect__empty) {
		KdTree tree;
		tree.build(10, 2);

		const Ray r(0, 0, 0);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 10);

		ASSERT_TRUE(found_objects.empty());
	}

	
	TEST(kdTree, intersect__leaf) {
		const Sprite s(0, 0, 64, 1, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(s);
		tree.build(10, 2);

		const Ray r(0, 0, 0);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 10);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(1, tree.objects.at(found_objects.front()).id);
	}
	
	TEST(kdTree, intersect__in_low) {
		const Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(-10, 0, PI);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(0, found_objects.front());
	}
	

	TEST(kdTree, intersect__in_high) {
		const Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(10, 0, 0);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(1, found_objects.front());
	}


	TEST(kdTree, intersect__across_the_split_from_low_to_high) {
		const Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(-10, 0, 0);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(0, found_objects.front());
		ASSERT_EQ(1, found_objects.back());
	}

	TEST(kdTree, intersect__across_the_split_from_high_to_low) {
		const Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(10, 0, PI);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(0, found_objects.front());
		ASSERT_EQ(1, found_objects.back());
	}

	TEST(kdTree, intersect__no_duplicates) {
		const Sprite both(0, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite low(-100, 0, 64, 2, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 3, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(both);
		tree.objects.emplace_back(high);
		tree.build(10, 2);
		
		const Ray r(10, 0, PI);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(3, found_objects.size());
		ASSERT_EQ(0, found_objects.at(0));
		ASSERT_EQ(1, found_objects.at(1));
		ASSERT_EQ(2, found_objects.at(2));
	}

	
	TEST(kdTree, intersect__ray_in_the_split) {
		const Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		const Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);
		
		const Ray r(0, 0, PI / 2);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(0, found_objects.front());
		ASSERT_EQ(1, found_objects.back());
	} 

	TEST(kdTree, intersect__split_on_z__across) {
		const Sprite low(0, -100, 64, 1, TextureIndex::ENEMY);
		const Sprite high(0, 100, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(0, 10, - PI / 2);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(0, found_objects.front());
		ASSERT_EQ(1, found_objects.back());
	}

	TEST(kdTree, intersect__split_on_z_only_high) {
		const Sprite low(0, -100, 64, 1, TextureIndex::ENEMY);
		const Sprite high(0, 100, 64, 2, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(low);
		tree.objects.emplace_back(high);
		tree.build(10, 1);

		const Ray r(0, 10, PI / 2);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(1, found_objects.back());
	}
	
	TEST(kdTree, intersect__a_very_complicated_tree) {
		const Sprite s1(70, 0, 32, 1, TextureIndex::ENEMY);
		const Sprite s2(110, 0, 32, 2, TextureIndex::ENEMY);
		const Sprite s3(140, 10, 32, 3, TextureIndex::ENEMY);
		const Sprite s4(110, 60, 32, 4, TextureIndex::ENEMY);
		const Sprite s5(70, 65, 32, 5, TextureIndex::ENEMY);
		const Sprite s6(130, 100, 32, 6, TextureIndex::ENEMY);
		const Sprite s7(70, 200, 32, 7, TextureIndex::ENEMY);
		const Sprite s8(125, 200, 32, 8, TextureIndex::ENEMY);

		KdTree tree;
		tree.objects.emplace_back(s1);
		tree.objects.emplace_back(s2);
		tree.objects.emplace_back(s3);
		tree.objects.emplace_back(s4);
		tree.objects.emplace_back(s5);
		tree.objects.emplace_back(s6);
		tree.objects.emplace_back(s7);
		tree.objects.emplace_back(s8);
		tree.build(10, 2);

		const Ray r(80, 100, + PI / 3);
		const std::vector<uint8_t> found_objects = tree.intersect(r, 200);

		ASSERT_EQ(5, found_objects.size()); // Indices 3 to 7.
	}
}