#pragma once

#include <memory>
#include <mutex>
#include <string>

#include <SDL.h>

#include "Canvas.h"
#include "Grid.h"
#include "Player.h"

namespace rc {

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

	class UserInterface : public Canvas
	{
	public:
		static const int SCREEN_WIDTH;
		static const int SCREEN_HEIGHT;

		/** Try not to create more than one! */
		UserInterface();
		~UserInterface();
		void openWindow();
		void game_loop(const Grid& world, Player& player);
		void set_wall_texture(const std::string& file_path);

		void draw_wall_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset) final;

	private:
		SDL_Window* main_window;
		SDL_Surface* main_window_surface;
		SDL_Renderer* renderer;

		bool halt_game_loop;
		std::unique_ptr<Image> wall_texture;

		UserInterface(const UserInterface&) = delete;
		void operator=(const UserInterface&) = delete;

		void poll_input(Player& player);
	};

}

