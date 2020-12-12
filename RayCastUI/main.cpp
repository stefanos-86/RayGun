#include "UserInterface.h"

#include <iostream>
#include <sstream>

#include "Grid.h"
#include "UserInterface.h"

// Just for the records: in 256 bytes on C64. http://www.pouet.net/prod.php?which=61298, https://www.youtube.com/watch?v=JxS0_ckSwqk


/** I am not going to implement code to load the world from a file. 
    This simple hardcode will do. */
std::stringstream fake_file_load() {
	std::stringstream level;


	/*level << 
		"x 10\n"
		"z 15\n"
		"cell_size 64\n"
		"##########\n"  //!!! Row 0 is here.
		"#....P...#\n"
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
		"#X...E...#\n"
		"##########\n"
		"player_start_orientation_rad 1.57\n"
		"player_ammo 30\n";*/
	
	/*level << 
		"x 34 "
		"z 11 "
		"cell_size 64 "
		// This is way less readable than I imagined at the beginning.
		"#############.########.##.###.###.\n"
		"#...........##.......P#..#...#X..#\n"
		"#.........#X#.....#####..#...#...#\n"
		"#.#.#.#.#.##.......#..#..#E..#...#\n"
		"#.........#...###...###.###.###.#.\n"
		"#E............###................#\n"
		"#.........#...#E#...###.###.###.#.\n"
		"#.#.#.#.#.##.......#.#...#...#...#\n"
		"#.........#.#E....#..#.E.#.E.#...#\n"
		"#........E#..#...#...#...#...#...#\n"
		".#########....###.....###.###.###.\n"
		"player_start_orientation_rad 3.14\n"
		"player_ammo 30\n"; 
		*/
		
	
	
	level <<
		"x 34 "
		"z 11 "
		"cell_size 64 "
		// This is way less readable than I imagined at the beginning.
		"#############E########E##E###E###E\n"
		"#EEEEEEEEEEE##EEEEEEEP#EE#EEE#XEE#\n"
		"#EEEEEEEEE#X#EEEEE#####EE#EEE#EEE#\n"
		"#E#E#E#E#E##EEEEEEE#EE#EE#EEE#EEE#\n"
		"#EEEEEEEEE#EEE###EEE###E###E###E#E\n"
		"#EEEEEEEEEEEEE###EEEEEEEEEEEEEEEE#\n"
		"#EEEEEEEEE#EEE#E#EEE###E###E###E#E\n"
		"#E#E#E#E#E##EEEEEEE#E#EEE#EEE#EEE#\n"
		"#EEEEEEEEE#E#EEEEE#EE#EEE#EEE#EEE#\n"
		"#EEEEEEEEE#EE#EEE#EEE#EEE#EEE#EEE#\n"
		"E#########EEEE###EEEEE###E###E###E\n"
		"player_start_orientation_rad 3.14\n"
		"player_ammo 30\n";
		
		

	/*
		level <<
		"x 4 "
		"z 3 "
		"cell_size 64 "
		"####\n"
		"#PE#\n"
		"####\n"
		"player_start_orientation_rad 0\n"
		"player_ammo 30\n";*/

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

		const rc::Sound sound_0("./impact.wav");
		const rc::Sound sound_1("./1_rec.wav");
		const rc::Sound sound_2("./2_rec.wav");
		const rc::Sound sound_3("./3_rec.wav");
		const rc::Sound sound_4("./4_rec.wav");

		ui.set_sound(rc::SoundIndex::GUN_SHOT, &sound_0);
		ui.set_sound(rc::SoundIndex::MUSIC_CALM, &sound_1);
		ui.set_sound(rc::SoundIndex::MUSIC_SLOW, &sound_2);
		ui.set_sound(rc::SoundIndex::MUSIC_MID, &sound_3);
		ui.set_sound(rc::SoundIndex::MUSIC_FAST, &sound_4);
		
		ui.set_sound_test(rc::SoundIndex::MUSIC_CALM, "./1_rec.wav");
		ui.set_sound_test(rc::SoundIndex::MUSIC_SLOW, "./2_rec.wav");

		ui.openWindow();

		ui.set_texture(rc::TextureIndex::WALL, "stone_wall.bmp");
		ui.set_texture(rc::TextureIndex::ENEMY, "bad_guy.bmp");
		ui.set_texture(rc::TextureIndex::FONT, "font.bmp");
		ui.set_texture(rc::TextureIndex::HUD, "gun.bmp");
		ui.set_texture(rc::TextureIndex::EXIT, "exit.bmp");

		ui.game_loop(world);
	}
	catch (std::runtime_error& x) {
		std::cerr << x.what() << std::endl;
	}
	/*catch (...) {
		// This block should probably do something more intelligent than this.
		std::cerr << "Unfathomable error." << std::endl;
	}*/
	return 0;
}