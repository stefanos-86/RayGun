#include "UserInterface.h"

#include <iostream>

#include "Grid.h"

void build_world(rc::Grid& g) {
	// Make a border of walls.
	for (uint8_t i = 0; i < g.x_size; ++i) {
		g.build_wall(i, 0);  // Bottom row.
		g.build_wall(i, g.z_size -1);  // Top row.
	}



	for (uint8_t j = 0; j < g.z_size; ++j) {
		g.build_wall(0, j);  // Left row.
		g.build_wall(g.x_size - 1, j); // Right row.
	}

	// "Spine" in the middle of the room... which I assume is 10s units long.
	for (uint8_t i = 3; i < 6; ++i) {
		g.build_wall(i, 4);
		g.build_wall(i, 5);
	}

	// Couple of random cubes.
	g.build_wall(3, 7);
	g.build_wall(7, 5);
}

int main(int argc, char* args[])
{
	try {
		// Used only to find where the images are supposed to go. #include <filesystem> to reuse.
		//std::cout << "Current path is " << std::filesystem::current_path() << std::endl;

		rc::Grid g(10, 10, 64);
		build_world(g);

		auto ui = rc::UserInterface::make_only_one();

		if (!ui)
			throw std::runtime_error("No UI!");

		ui->openWindow();
		//ui->set_wall_texture("test.bmp");
		ui->set_wall_texture("stone_wall.bmp");
		ui->game_loop(g);

	}
	catch (std::runtime_error& x) {
		std::cerr << x.what() << std::endl;
	}
	return 0;
}