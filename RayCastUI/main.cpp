#include "UserInterface.h"

#include <iostream>

#include "Grid.h"
#include "UserInterface.h"

/** I am not going to implement code to load the world from a file. 
    This simple hardcode will do. */
rc::Grid fake_load_level() {
	rc::Grid g(10, 15, 64);

	// Make a border of walls.
	for (uint8_t i = 0; i < g.x_size; ++i) {
		g.build_wall(i, 0);  // Bottom row.
		g.build_wall(i, g.z_size -1);  // Top row.
	}

	for (uint8_t j = 0; j < g.z_size; ++j) {
		g.build_wall(0, j);  // Left row.
		g.build_wall(g.x_size - 1, j); // Right row.
	}

	// "Spine" in the middle of the 1st room.
	for (uint8_t i = 3; i < 6; ++i) {
		g.build_wall(i, 4);
		g.build_wall(i, 5);
	}

	// Couple of cubes "alone" in the 1st room.
	g.build_wall(3, 7);
	g.build_wall(7, 5);

	// Wall between rooms, skipping a door.
	const uint8_t door_break = g.x_size / 2 - 1;
	for (uint8_t i = 0; i < door_break; ++i)
		g.build_wall(i, 10);
	for (uint8_t i = door_break + 2; i < g.x_size; ++i)
		g.build_wall(i, 10);

	return g;
}

int main(int argc, char* args[])
{
	try {
		// Used only to find where the image files are supposed to go. #include <filesystem> to reuse.
		//std::cout << "Current path is " << std::filesystem::current_path() << std::endl;

		rc::World world{
			fake_load_level(),
			{335, 862, 4.7f},  // "Fake load player", hardcode it to a "nice" position in the world.
			{ { {320.0f, 512.0f, 64}, {192.0f, 192.0f, 64} } } // Here are the enemies.
		};

		rc::UserInterface ui;
		ui.openWindow();

		ui.set_texture(rc::TextureIndex::WALL, "stone_wall.bmp");
		ui.set_texture(rc::TextureIndex::ENEMY, "bad_guy.bmp");

		ui.game_loop(world);
	}
	catch (std::runtime_error& x) {
		std::cerr << x.what() << std::endl;
	}
	catch (...) {
		// This block should probably do something more intelligent than this.
		std::cerr << "Unfathomable error." << std::endl;
	}
	return 0;
}