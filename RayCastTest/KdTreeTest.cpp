#include "pch.h"

#include "KdTree.h"

#include <memory>
#include <vector>

#include "PI.h"
#include "Ray.h"
#include "Sprite.h"


// TODO: change all the expect into assert.

#define EXPECT_NULL(p) EXPECT_FALSE(p)
#define EXPECT_NOT_NULL(p) EXPECT_TRUE(p)

namespace rc {
	TEST(kdTree, Build__empty) {
		std::vector<const Sprite*> nothing;

		std::unique_ptr<KdTree> tree = KdTree::build(nothing, 10, 2);

		EXPECT_NOT_NULL(tree);
		EXPECT_NULL(tree->low);
		EXPECT_NULL(tree->high);
		EXPECT_TRUE(tree->content.empty());
	}

	TEST(kdTree, Build__one_element) {
		const uint8_t sprite_id = 1;
		Sprite s(0, 0, 64, sprite_id, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&s);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 2);

		EXPECT_NOT_NULL(tree);
		EXPECT_NULL(tree->low);
		EXPECT_NULL(tree->high);
		EXPECT_EQ(1, tree->content.size());
		EXPECT_EQ(sprite_id, tree->content.front()->id);
	}

	TEST(kdTree, Build__two_elements__split) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);  // Each node can have only 1 element.

		EXPECT_EQ(tree->partition_direction, KdTree::Partition::ON_X);
		EXPECT_NOT_NULL(tree->low);
		EXPECT_NOT_NULL(tree->high);
		EXPECT_EQ(1, tree->low->content.size());
		EXPECT_EQ(1, tree->low->content.front()->id);
		EXPECT_EQ(1, tree->high->content.size());
		EXPECT_EQ(2, tree->high->content.front()->id);
	}


	TEST(kdTree, Build__element_in_the_middle_goes_both_sides) {
		Sprite both(0, 0, 64, 1, TextureIndex::ENEMY);
		Sprite low(-100, 0, 64, 2, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 3, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&both);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 2);

		EXPECT_EQ(tree->partition_direction, KdTree::Partition::ON_X);
		EXPECT_NOT_NULL(tree->low);
		EXPECT_NOT_NULL(tree->high);
		EXPECT_EQ(2, tree->low->content.size());
		EXPECT_EQ(1, tree->low->content.back()->id);
		EXPECT_EQ(2, tree->high->content.size());
		EXPECT_EQ(1, tree->high->content.front()->id);
	}
	

	TEST(kdTree, Build__split_two_levels) {
		Sprite left_down(-200, -100, 64, 1, TextureIndex::ENEMY);
		Sprite left_up(-200, 100, 64, 2, TextureIndex::ENEMY);
		Sprite right_down(200, -100, 64, 3, TextureIndex::ENEMY);
		Sprite right_up(200, 100, 64, 4, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&left_down);
		objects.push_back(&left_up);
		objects.push_back(&right_down);
		objects.push_back(&right_up);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		// Sad jungle of assertions, but has to explore the whole tree.

		EXPECT_EQ(tree->partition_direction, KdTree::Partition::ON_X);
		EXPECT_NOT_NULL(tree->low);
		EXPECT_NOT_NULL(tree->high);
		const KdTree& low_x = *(tree->low);
		const KdTree& high_x = *(tree->high);
		
		EXPECT_EQ(low_x.partition_direction, KdTree::Partition::ON_Z);
		EXPECT_NOT_NULL(low_x.low);
		EXPECT_NOT_NULL(low_x.high);
		EXPECT_EQ(1, low_x.low->content.front()->id);
		EXPECT_EQ(2, low_x.high->content.front()->id);


		EXPECT_EQ(high_x.partition_direction, KdTree::Partition::ON_Z);
		EXPECT_NOT_NULL(high_x.low);
		EXPECT_NOT_NULL(high_x.high);
		EXPECT_EQ(3, high_x.low->content.front()->id);
		EXPECT_EQ(4, high_x.high->content.front()->id);
	}

	TEST(kdTree, intersect__empty) {
		std::vector<const Sprite*> nothing;
		std::unique_ptr<KdTree> tree = KdTree::build(nothing, 10, 2);

		Ray r(0, 0, 0);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 10);

		EXPECT_TRUE(found_objects.empty());
	}


	TEST(kdTree, intersect__leaf) {
		Sprite s(0, 0, 64, 1, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&s);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 2);

		Ray r(0, 0, 0);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 10);

		EXPECT_EQ(found_objects, objects);
	}

	TEST(kdTree, intersect__in_low) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);  // Each node can have only 1 element.
	
		Ray r(-10, 0, PI);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(1, found_objects.front()->id);
	}


	TEST(kdTree, intersect__in_high) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);  // Each node can have only 1 element.

		Ray r(10, 0, 0);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(2, found_objects.front()->id);
	}


	TEST(kdTree, intersect__across_the_split_from_low_to_high) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		Ray r(-10, 0, 0);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(2, found_objects.front()->id);
		ASSERT_EQ(1, found_objects.back()->id);
	}

	TEST(kdTree, intersect__across_the_split_from_high_to_low) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		Ray r(10, 0, PI);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(2, found_objects.front()->id);
		ASSERT_EQ(1, found_objects.back()->id);
	}

	TEST(kdTree, intersect__no_duplicates) {
		Sprite both(0, 0, 64, 1, TextureIndex::ENEMY);
		Sprite low(-100, 0, 64, 2, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 3, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&both);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 2);

		Ray r(10, 0, PI);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(3, found_objects.size());
		ASSERT_EQ(1, found_objects.front()->id);
		ASSERT_EQ(2, found_objects.back()->id);
		ASSERT_EQ(2, found_objects.back()->id);
	}


	TEST(kdTree, intersect__ray_in_the_split) {
		Sprite low(-100, 0, 64, 1, TextureIndex::ENEMY);
		Sprite high(+100, 0, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		Ray r(0, 0, PI / 2);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(2, found_objects.front()->id);
		ASSERT_EQ(1, found_objects.back()->id);
	} 

	TEST(kdTree, intersect__split_on_z__across) {
		Sprite low(0, -100, 64, 1, TextureIndex::ENEMY);
		Sprite high(0, 100, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		Ray r(0, 10, - PI / 2);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(2, found_objects.size());
		ASSERT_EQ(2, found_objects.front()->id);
		ASSERT_EQ(1, found_objects.back()->id);
	}

	TEST(kdTree, intersect__split_on_z_only_high) {
		Sprite low(0, -100, 64, 1, TextureIndex::ENEMY);
		Sprite high(0, 100, 64, 2, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&low);
		objects.push_back(&high);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 1);

		Ray r(0, 10, PI / 2);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(1, found_objects.size());
		ASSERT_EQ(2, found_objects.back()->id);
	}
	
	TEST(kdTree, intersect__a_very_complicated_tree) {
		Sprite s1(70, 0, 32, 1, TextureIndex::ENEMY);
		Sprite s2(110, 0, 32, 2, TextureIndex::ENEMY);
		Sprite s3(140, 10, 32, 3, TextureIndex::ENEMY);
		Sprite s4(110, 60, 32, 4, TextureIndex::ENEMY);
		Sprite s5(70, 65, 32, 5, TextureIndex::ENEMY);
		Sprite s6(130, 100, 32, 6, TextureIndex::ENEMY);
		Sprite s7(70, 200, 32, 7, TextureIndex::ENEMY);
		Sprite s8(125, 200, 32, 8, TextureIndex::ENEMY);

		std::vector<const Sprite*> objects;
		objects.push_back(&s1);
		objects.push_back(&s2);
		objects.push_back(&s3);
		objects.push_back(&s4);
		objects.push_back(&s5);
		objects.push_back(&s6);
		objects.push_back(&s7);
		objects.push_back(&s8);

		std::unique_ptr<KdTree> tree = KdTree::build(objects, 10, 2);

		Ray r(80, 100, + PI / 3);
		std::vector<const Sprite*> found_objects = tree->intersect(r, 200);

		ASSERT_EQ(4, found_objects.size()); // IDs are 4, 6, 7, 8.
	}
	
}