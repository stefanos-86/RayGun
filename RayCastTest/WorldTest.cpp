#include "pch.h"

#include "World.h"

#include <sstream>

// TODO: not sure I can get the same effect of "piece of a multi-literal string" with a constexpr.
#define PLAYER_NOT_IMPORTANT "player_start_x 0 player_start_z 0 player_start_orientation_rad 0 player_ammo 0\n"

namespace rc {

    TEST(World, load__no_data) {
        std::stringstream nothing;
        ASSERT_ANY_THROW(World::load(nothing));
    }

    TEST(World, load__map_minimal_grid) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z 1\n"
            ".\n"
            PLAYER_NOT_IMPORTANT;

        const Grid g = World::load(world_text).map;

        ASSERT_EQ(1, (int)g.x_size);
        ASSERT_EQ(1, (int)g.z_size);
        ASSERT_FALSE(g.wall_at(0, 0));
    }

    TEST(World, load__map_minimal_wall) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z 1\n"
            "#\n"
            PLAYER_NOT_IMPORTANT;

        const Grid g = World::load(world_text).map;

        ASSERT_TRUE(g.wall_at(0, 0));
    }

    TEST(World, load__map_bad_size_z) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z -1\n"
            ".\n"
            PLAYER_NOT_IMPORTANT;

        ASSERT_ANY_THROW(World::load(world_text));
    }

    TEST(World, load__map_bad_size_x) {
        std::stringstream world_text;
        world_text <<
            "x 257\n"
            "z 1\n"
            ".\n"
            PLAYER_NOT_IMPORTANT;

        ASSERT_ANY_THROW(World::load(world_text));
    }

    TEST(World, load__map_invalid_char) {
        std::stringstream world_text;
        world_text <<
            "x 257\n"
            "z 1\n"
            "P\n"
            PLAYER_NOT_IMPORTANT;  // Not ever reached, but must give correct input in this part.

        ASSERT_ANY_THROW(World::load(world_text));
    }

    TEST(World, load__map_row) {
        std::stringstream world_text;
        world_text <<
            "x 3\n"
            "z 1\n"
            "#.#\n"
            PLAYER_NOT_IMPORTANT;

        const Grid g = World::load(world_text).map;

        ASSERT_EQ(3, (int)g.x_size);
        ASSERT_EQ(1, (int)g.z_size);

        ASSERT_TRUE(g.wall_at(0, 0));
        ASSERT_FALSE(g.wall_at(1, 0));
        ASSERT_TRUE(g.wall_at(2, 0));
    }

    TEST(World, load__map_column) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z 3\n"
            "#\n"
            ".\n"
            "#\n"
            PLAYER_NOT_IMPORTANT;

        const Grid g = World::load(world_text).map;

        ASSERT_EQ(1, (int)g.x_size);
        ASSERT_EQ(3, (int)g.z_size);

        ASSERT_TRUE(g.wall_at(0, 0));
        ASSERT_FALSE(g.wall_at(0, 1));
        ASSERT_TRUE(g.wall_at(0, 2));
    }

    TEST(World, load__map_square) {
        std::stringstream world_text;
        world_text <<
            "x 4\n"
            "z 4\n"
            "####\n"
            "#..#\n"
            "#..#\n"
            "####\n"
            PLAYER_NOT_IMPORTANT;

        const Grid g = World::load(world_text).map;

        // "Light" checks. Just the corners.
        ASSERT_TRUE(g.wall_at(0, 0));
        ASSERT_TRUE(g.wall_at(3, 3));
    }

    TEST(World, load__player) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z 1\n"
            ".\n"
            "player_start_x 31.5\n"
            "player_start_z 32\n"
            "player_start_orientation_rad 12.5\n"
            "player_ammo 30\n";  // TODO: what happens if I put a float here? Or a letter? Or nothing at all?

        const Player p = World::load(world_text).player;

        ASSERT_EQ(31.5, p.x_position);
        ASSERT_EQ(32, p.z_position);
        ASSERT_EQ(12.5, p.orientation);
        ASSERT_EQ(30, p.bullets_left);
    }


    TEST(World, load__enemies_just_one) {
        std::stringstream world_text;
        world_text <<
            "x 1\n"
            "z 1\n"
            "E\n"
            PLAYER_NOT_IMPORTANT;

        const Enemies e = World::load(world_text).enemies;

        ASSERT_EQ(1, e.sprites.size());
        // Sprite x and z are private. I would have to test via the intersection, but that is complicated...
    }
    
    // TODO: test what happens if grid test goes outside grid size. Or if the grid is missing, has holes (\n\n) etc.
    // TODO: try to comment in the stream. Use ; as a separator (saves # for the walls and it is the assembler convention).
}