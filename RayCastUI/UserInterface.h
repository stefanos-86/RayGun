#pragma once

#include <string>
#include <unordered_map>

#include <SDL.h>

#include "Canvas.h"
#include "World.h"

namespace rc {


	/** Just a wrapper to hold the SDL structures for a texture and ensure RAII clean up. */
	class Image {
	public:
		Image(const std::string& file_path, SDL_Renderer* renderer);
		~Image();

		/** Required to allow creation via STL containers emplace methods. */
		Image(Image&& other) noexcept;

		/** Required to allow creation via STL containers emplace methods. */
		Image& operator=(Image&& other) noexcept;

		/** Returns true if the pixel at the given coordinates has a small alpha. */
		bool transparent_pixel(const uint8_t x, const uint8_t y) const;

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
		void game_loop(World& world);

		void set_texture(const TextureIndex name, const std::string& file_path);

		void draw_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset, const TextureIndex what_to_draw) final;

		bool transparent_pixel(const uint8_t x, const uint8_t y, const TextureIndex image) const final;

		/** I am well aware of http://www.libsdl.org/projects/SDL_ttf/, but I am trying to link in as
		* little as possible. */
		void draw_text(const std::string& text, uint16_t row, const int16_t column, const uint8_t font_size) const;

	private:
		SDL_Window* main_window;
		SDL_Surface* main_window_surface;
		SDL_Renderer* renderer;

		bool halt_game_loop;
		bool pause_game_loop;

		std::unordered_map<TextureIndex, Image> textures;

		UserInterface(const UserInterface&) = delete;
		void operator=(const UserInterface&) = delete;

		void poll_input(World& world);

		/** Cleans the frame buffer, draws the ceiling.
		Striclty speaking, this class should only offer primitives to do so, and let the 
		"game logic" tell it what to draw and when. But since there is NO game logic to 
		do it and the visual effect is nice, I break the single responsibility principle
		and "dump" this little bit of graphics here. */
		void draw_background();

		/** Color in red the pixel in the middle of the screen.
		    This is meant as a debug aid, to see what the player is pointing at. */
		void draw_debug_crosshair();
	};

}
