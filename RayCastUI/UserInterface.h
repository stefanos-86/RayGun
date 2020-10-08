#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <SDL.h>

#include "Canvas.h"
#include "Grid.h"
#include "Player.h"

namespace rc {


	/** Just a wrapper to hold the SDL structures for a texture and ensure RAII clean up. */
	class Image {
	public:
		Image(const std::string& file_path, SDL_Renderer* renderer);
		~Image();

		SDL_Surface* surface;
		SDL_Texture* texture;
	
	private:
		// Avoid copy, would make a mess with the pointers.
		Image(const Image& other) = delete;
		void operator=(const Image&) = delete;
	};


	/** This class is the entry point to visualize things and react to keys.
	    "Glues" the SDL calls and the rest of the game, ensures RAII handling of the SDL
		data.

		It implements the Canvas interface so that it can be called back by the projection plane
		(it is a dependency inversion).
	*/
	class UserInterface : public Canvas
	{
	public:
		static constexpr int SCREEN_WIDTH = 640;
		static constexpr int SCREEN_HEIGHT = 480;

		/** Try not to create more than one! It instantiates SDL structures on creation.*/
		UserInterface();
		~UserInterface();

		void openWindow();
		void game_loop(const Grid& world, Player& player);
		void set_wall_texture(const std::string& file_path);
		void set_enemy_texture(const std::string& file_path);

		void draw_wall_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset) final;
		void draw_enemy_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset) final;

	private:
		SDL_Window* main_window;
		SDL_Surface* main_window_surface;
		SDL_Renderer* renderer;

		bool halt_game_loop;
		std::unique_ptr<Image> wall_texture;
		std::unique_ptr<Image> enemy_texture;

		UserInterface(const UserInterface&) = delete;
		void operator=(const UserInterface&) = delete;

		void poll_input(Player& player);

		/** Cleans the frame buffer, draws the ceiling.
		Striclty speaking, this class should only offer primitives to do so, and let the 
		"game logic" tell it what to draw and when. But since there is NO game logic to 
		do it and the visual effect is nice, I break the single responsibility principle
		and "dump" this little bit of graphics here. */
		void draw_background();
	};

}

