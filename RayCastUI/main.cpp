#include "UserInterface.h"

#include <iostream>
#include <sstream>

#include "Grid.h"
#include "UserInterface.h"

// TODO Audio?
// https://github.com/jakebesworth/Simple-SDL2-Audio/tree/master/src
// https://gist.github.com/armornick/3447121


/** I am not going to implement code to load the world from a file. 
    This simple hardcode will do. */
std::stringstream fake_file_load() {
	std::stringstream level;
	level << 
		"x 10\n"
		"z 15\n"
		"##########\n"  //!!! Row 0 is here.
		"#........#\n"
		"#........#\n"
		"#........#\n"
		"####..####\n"
		"#........#\n"
		"#........#\n"
		"#..#.....#\n"
		"#....E.#.#\n"
		"#........#\n"
		"#..####..#\n"
		"#....E...#\n"
		"#....E...#\n"
		"#....E...#\n"
		"##########\n"
		"player_start_x 335\n"
		"player_start_z 80\n"
		"player_start_orientation_rad 1.57\n"
		"player_ammo 30\n";

	return level;
}

int main(int argc, char* args[])
{
	try {
		// Used only to find where the image files are supposed to go. #include <filesystem> to reuse.
		//std::cout << "Current path is " << std::filesystem::current_path() << std::endl;

		std::stringstream level_file = fake_file_load();
		rc::World world = rc::World::load(level_file);

		rc::UserInterface ui;
		ui.openWindow();

		ui.set_texture(rc::TextureIndex::WALL, "stone_wall.bmp");
		ui.set_texture(rc::TextureIndex::ENEMY, "bad_guy.bmp");
		ui.set_texture(rc::TextureIndex::FONT, "font.bmp");

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