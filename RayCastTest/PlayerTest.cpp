#include "pch.h"

#include "Player.h"

#include "PI.h"

namespace rc {

	TEST(Player, advance__horizontal) {
		Player p{ 0, 0, 0 };

		p.advance(+1);
		ASSERT_FLOAT_EQ(10, p.x_position);
		ASSERT_FLOAT_EQ(0, p.z_position);
	}

	TEST(Player, advance__vertical) {
		Player p{ 0, 0, PI / 2 };

		p.advance(+1);
		ASSERT_FLOAT_EQ(10, p.z_position);
		ASSERT_FLOAT_EQ((float)-4.371139e-07, p.x_position);  //TODO: "Almost", but not quite, 0...
	}

	TEST(Player, advance__backward) {
		Player p{ 10, 0, 0 };

		p.advance(-1);
		ASSERT_FLOAT_EQ(0, p.x_position);
		ASSERT_FLOAT_EQ(0, p.z_position);
	}

	TEST(Player, advance__reverse_orientation) {
		Player p{ 10, 0, PI};

		p.advance(1);
		ASSERT_FLOAT_EQ(0, p.x_position);
		ASSERT_FLOAT_EQ(-8.7422779e-07, p.z_position);
	}

	TEST(Player, advance__reverse_orientation_backward) {
		Player p{ 10, 0, PI };

		p.advance(-1);
		ASSERT_FLOAT_EQ(20, p.x_position);
		ASSERT_FLOAT_EQ(8.7422779e-07, p.z_position);
	}

	TEST(Player, advance__diagonal) {
		Player p{ 10, 0, 3 * PI / 4};

		p.advance(1);
		ASSERT_FLOAT_EQ(2.9289322, p.x_position);
		ASSERT_FLOAT_EQ(7.0710678, p.z_position);
	}

	TEST(Player, advance__diagonal_reverse) {
		Player p{ 10, 0, 3 * PI / 4 };

		p.advance(-1);
		ASSERT_FLOAT_EQ(17.071068, p.x_position);
		ASSERT_FLOAT_EQ(-7.0710678, p.z_position);
	}


	TEST(Player, turn__left) {
		Player p{ 0, 0, 0 };

		p.turn(1);
		ASSERT_FLOAT_EQ(0.05f, p.orientation);
	}

	TEST(Player, turn__right) {
		Player p{ 0, 0, 0 };

		p.turn(-1);
		ASSERT_FLOAT_EQ(-0.05f, p.orientation);
	}
}