#include "pch.h"

#include "Player.h"

#include "Grid.h"
#include "PI.h"

namespace rc {

	static const Grid empty_map(10, 10, 64);

	TEST(Player, advance__horizontal) {
		Player p{ 0, 0, 0 };


		p.advance(+1, empty_map);
		ASSERT_FLOAT_EQ(5, p.x_position);
		ASSERT_FLOAT_EQ(0, p.z_position);
	}

	TEST(Player, advance__vertical) {
		Player p{ 0, 0, PI / 2 };

		p.advance(+1, empty_map);
		ASSERT_FLOAT_EQ(5.0f, p.z_position);
		ASSERT_FLOAT_EQ(-2.1855695e-07f, p.x_position);
	}

	TEST(Player, advance__backward) {
		Player p{ 5, 0, 0 };

		p.advance(-1, empty_map);
		ASSERT_FLOAT_EQ(0, p.x_position);
		ASSERT_FLOAT_EQ(0, p.z_position);
	}

	TEST(Player, advance__reverse_orientation) {
		Player p{ 5, 0, PI};

		p.advance(1, empty_map);
		ASSERT_FLOAT_EQ(0.0f, p.x_position);
		ASSERT_FLOAT_EQ(-4.371139e-07f, p.z_position);
	}

	TEST(Player, advance__reverse_orientation_backward) {
		Player p{ 15, 0, PI };

		p.advance(-1, empty_map);
		ASSERT_FLOAT_EQ(20.0f, p.x_position);
		ASSERT_FLOAT_EQ(4.371139e-07f, p.z_position);
	}

	TEST(Player, advance__diagonal) {
		Player p{ 5, 0, 3 * PI / 4};

		p.advance(1, empty_map);
		ASSERT_FLOAT_EQ(1.4644661f, p.x_position);
		ASSERT_FLOAT_EQ(3.5355339f, p.z_position);
	}

	TEST(Player, advance__diagonal_reverse) {
		Player p{ 10, 0, 3 * PI / 4 };

		p.advance(-1, empty_map);
		ASSERT_FLOAT_EQ(13.535534f, p.x_position);
		ASSERT_FLOAT_EQ(-3.5355339f, p.z_position);
	}


	TEST(Player, turn__left) {
		Player p{ 0, 0, 0 };

		p.turn(1);
		ASSERT_FLOAT_EQ(0.045000002f, p.orientation);
	}

	TEST(Player, turn__right) {
		Player p{ 0, 0, 0 };

		p.turn(-1);
		ASSERT_FLOAT_EQ(6.2831855f, p.orientation);
	}

	// TODO: test movement that hits walls.
}