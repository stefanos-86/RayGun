#include "UserInterface.h"

#include <stdexcept>

#include "ProjectionPlane.h"

namespace rc {

	static void sdl_null_check(const void* pointer) {
		if (!pointer)
			throw std::runtime_error(SDL_GetError());
	}

	static void sdl_return_check(const int rc) {
		if (rc != 0)
			throw std::runtime_error(SDL_GetError());
	}

	Image::Image(const std::string& file_path, SDL_Renderer* renderer)
	{
		surface = SDL_LoadBMP(file_path.c_str());  // Assuming it handles bad files.
		sdl_null_check(surface);

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		sdl_null_check(texture);
	}

	Image::~Image()
	{
		// SDL can handle the nullptrs, but the docs says it would set an error message.
		// Do it by hand and leave it clean.
		if (texture) SDL_DestroyTexture(texture);
		if (surface) SDL_FreeSurface(surface);
	}

	Image::Image(Image&& other) noexcept:
		surface(other.surface),
		texture(other.texture)
	{
		other.surface = nullptr;
		other.texture = nullptr;
	}

	Image& Image::operator=(Image&& other) noexcept
	{

		this->surface = other.surface;
		this->texture = other.texture;

		other.surface = nullptr;
		other.texture = nullptr;

		return *this;
	}


	/** Implementation stolen from https://stackoverflow.com/questions/53033971/how-to-get-the-color-of-a-specific-pixel-from-sdl-surface. */
	bool Image::transparent_pixel(const uint8_t x, const uint8_t y) const
	{
		const Uint8 bpp = surface->format->BytesPerPixel;
		const Uint8* pixel = (Uint8*) surface->pixels + y * surface->pitch + x * bpp;

		Uint32 pixel_value = 0;
		switch (bpp)
		{
		case 1:
			pixel_value = *pixel;
			break;
		case 2:
			pixel_value = *(Uint16*) pixel;
			break;
		case 3:
			pixel_value = (SDL_BYTEORDER == SDL_BIG_ENDIAN) ?
					pixel[0] << 16 | pixel[1] << 8 | pixel[2] :
					pixel[0] | pixel[1] << 8 | pixel[2] << 16;
			break;
		case 4:
			pixel_value = *(Uint32*) pixel;
			break;
		default:
			throw std::runtime_error("Impossible bit per pixel.");
		}

		uint8_t r, g, b;
		uint8_t alpha;
		SDL_GetRGBA(pixel_value, surface->format, &r, &g, &b, &alpha);

		return alpha < 128;
	}
	

	UserInterface::UserInterface() :
		main_window(nullptr),
		main_window_surface(nullptr),
		renderer(nullptr),
		halt_game_loop(true),  // Safe default.
		pause_game_loop(false)
	{
	}

	UserInterface::~UserInterface()
	{
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(main_window);
		SDL_Quit();
	}


	void UserInterface::openWindow()
	{
		if (SDL_Init(SDL_INIT_VIDEO) < 0)
			throw std::runtime_error(SDL_GetError());

		main_window = SDL_CreateWindow(
			"Ray Cast Exercise", 
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
			UserInterface::SCREEN_WIDTH, UserInterface::SCREEN_HEIGHT,
			SDL_WINDOW_SHOWN);
		sdl_null_check(main_window);

		main_window_surface = SDL_GetWindowSurface(main_window);
		sdl_null_check(main_window_surface);

		renderer = SDL_CreateRenderer(main_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
		sdl_null_check(renderer);
	}
	
	void UserInterface::poll_input(World& world)
	{
		Player& player = world.player;
		const Grid& map = world.map;

		SDL_Event user_input;
		while (SDL_PollEvent(&user_input) != 0)
			if (user_input.type == SDL_QUIT)
				halt_game_loop = true;
			else if (user_input.type == SDL_KEYDOWN) {
				if (user_input.key.keysym.scancode == SDL_SCANCODE_P)
					pause_game_loop = !pause_game_loop;
				else if (user_input.key.keysym.scancode == SDL_SCANCODE_SPACE && !pause_game_loop)
					// Cheap way out to avoid a cooldown timer on the shoot key. Using the key state
					// would cause a shot per frame (too fast).
					player.shoot(map, world.enemies, *this);
			}

		if (pause_game_loop)
			return;
			// Ignore any other input.
		    // TODO: this causes a CPU usage spike! Probably it is furiously polling the input.
		

		// Intentionally ignore nonsensical key combos (e. g. up and down at the same time).
		const Uint8* key_states = SDL_GetKeyboardState(nullptr);
		if (key_states[SDL_SCANCODE_UP])
			player.advance(1, map);
		else if (key_states[SDL_SCANCODE_DOWN])
			player.advance(-1, map);
		
		if (key_states[SDL_SCANCODE_LEFT])
			player.turn(-1);
		else if (key_states[SDL_SCANCODE_RIGHT])
			player.turn(+1);

		if (key_states[SDL_SCANCODE_ESCAPE])
			halt_game_loop = true;
	}

	void UserInterface::draw_background() {
		int rc = 0;

		rc = SDL_SetRenderDrawColor(renderer, 150, 150, 150, 255);
		sdl_return_check(rc);
		
		rc = SDL_RenderClear(renderer);
		sdl_return_check(rc);

		// Draw the floor darker than the ceiling and the texture. It looks better.
		rc = SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
		sdl_return_check(rc);

		SDL_Rect half_screen;
		half_screen.x = 0;
		half_screen.y = UserInterface::SCREEN_HEIGHT / 2;
		half_screen.h = UserInterface::SCREEN_HEIGHT / 2;
		half_screen.w = UserInterface::SCREEN_WIDTH;

		rc = SDL_RenderFillRect(renderer, &half_screen);
		sdl_return_check(rc);
	}

	void UserInterface::draw_debug_crosshair() {
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		SDL_RenderDrawPoint(renderer, UserInterface::SCREEN_WIDTH / 2, UserInterface::SCREEN_HEIGHT / 2);
	}

	void UserInterface::game_loop(World& world)
	{
		ProjectionPlane projection(UserInterface::SCREEN_WIDTH, UserInterface::SCREEN_HEIGHT, 60);

		halt_game_loop = false;
		while (! halt_game_loop) {
			poll_input(world);

			if (halt_game_loop)
				return;

			if (pause_game_loop) {
				world.hud.alert_pause(*this);
			}
			else
			{
				draw_background();
				projection.project_objects(world, *this);
				draw_debug_crosshair();
				world.hud.display(world.player, *this);
			}

			SDL_RenderPresent(renderer);
		}
	}


	void UserInterface::set_texture(const TextureIndex name, const std::string& file_path)
	{
		textures.emplace(name, Image(file_path, renderer));
	}

	void UserInterface::draw_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset, const TextureIndex what_to_draw)
	{
		const Image& texture = textures.at(what_to_draw);

		SDL_Rect source_slice;
		source_slice.x = texture_offset;
		source_slice.y = 0;
		source_slice.w = 1;
		source_slice.h = texture.surface->w;  // Assume widht and height match.

		SDL_Rect dest_slice;
		dest_slice.x = column;
		dest_slice.y = top_row;
		dest_slice.w = 1;
		dest_slice.h = height;

		const int rc = SDL_RenderCopy(renderer, texture.texture, &source_slice, &dest_slice);  // TODO Or maybe I have to use SDL_BlitSurface?
		sdl_return_check(rc);
	}


	// TODO: can I avoid this indirection? "Link" the image into the sprites?
	bool UserInterface::transparent_pixel(const uint8_t x, const uint8_t y, const TextureIndex image) const
	{
		return textures.at(image).transparent_pixel(x, y);
	}


	/** Assumes a 64*64 bitmap with the letters, 8*8 pixels each.
	    Numbers 0 to 9, then uppercase letters, then ! and : and then... stop. I don't need anything else. 

		I am aware of http://www.libsdl.org/projects/SDL_ttf/, but I am trying to link in as
		little extra libraries as possible. */
	void UserInterface::draw_text(const std::string& text, uint16_t column, const uint16_t row, const uint8_t font_size)
	{
		constexpr uint8_t source_letter_side = 8;
		constexpr uint8_t last_char_bitmap = 64;

		const Image& texture = textures.at(TextureIndex::FONT);
		uint16_t cursor = column;

		// Remap the ASCII code to the bitmap position.
		for (char c : text) {
			if ('A' <= c && c <= 'Z')
				c = c - 'A' + 18;  // Letters starts after digits and some spave and 'A' is the 1st letter in the bitmap.
			else if ('0' <= c && c <= ':')  // The ':' is just after '9' in ASCII.
				c -= '0';
			else if (c == '!')
				c = 44; // I know where this is on the bitmap...
			else //Space or unsupported char, skip and leave a space.
				c = last_char_bitmap + 1;

			if (c < last_char_bitmap) {
				// Compute char position in terms of pixels in the bitmap.
				const uint8_t font_row = c / source_letter_side * source_letter_side;
				const uint8_t font_column = c % source_letter_side * source_letter_side;

				SDL_Rect source_slice;
				source_slice.x = font_column;
				source_slice.y = font_row;
				source_slice.w = source_letter_side;
				source_slice.h = source_letter_side;

				SDL_Rect dest_slice;
				dest_slice.x = cursor;
				dest_slice.y = row;
				dest_slice.w = font_size;
				dest_slice.h = font_size;

				const int rc = SDL_RenderCopy(renderer, texture.texture, &source_slice, &dest_slice);  // TODO Or maybe I have to use SDL_BlitSurface?
				sdl_return_check(rc);
			}
			cursor += font_size;
		}
	}

	void UserInterface::draw_image(uint16_t column_x, const uint16_t row_y, const TextureIndex what_to_draw)
	{
		const Image& texture = textures.at(what_to_draw);

		SDL_Rect source_slice;
		source_slice.x = 0;
		source_slice.y = 0;
		source_slice.w = texture.surface->w;
		source_slice.h = texture.surface->h;


		// TODO: scale with the screen...
		SDL_Rect dest_slice;
		dest_slice.x = column_x;
		dest_slice.y = row_y;
		dest_slice.w = texture.surface->w;
		dest_slice.h = texture.surface->h;

		const int rc = SDL_RenderCopy(renderer, texture.texture, &source_slice, &dest_slice);  // TODO Or maybe I have to use SDL_BlitSurface?
		sdl_return_check(rc);
	}

}