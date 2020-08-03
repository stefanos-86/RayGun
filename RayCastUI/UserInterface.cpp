#include "UserInterface.h"

#include <stdexcept>

#include "ProjectionPlane.h"

#include <iostream>

namespace rc {

	static void sdl_null_check(const void* pointer) {
		if (!pointer)
			throw std::runtime_error(SDL_GetError());
	}

	Image::Image(const std::string& file_path, SDL_Renderer* renderer)
	{
		surface = SDL_LoadBMP(file_path.c_str());  // Assumes it handles bad files.
		sdl_null_check(surface);

		texture = SDL_CreateTextureFromSurface(renderer, surface);
		sdl_null_check(texture);
	}

	Image::~Image()
	{
		SDL_DestroyTexture(texture);
		SDL_FreeSurface(surface);
	}


	const int UserInterface::SCREEN_WIDTH = 640;
	const int UserInterface::SCREEN_HEIGHT = 480;

	UserInterface::UserInterface() :
		main_window(nullptr),
		main_window_surface(nullptr),
		renderer(nullptr),
		halt_game_loop(true)  // Safe default.
	{
	}

	/** Overkill? */
	std::unique_ptr<UserInterface> UserInterface::make_only_one()
	{
		static std::mutex avoid_concurrent_creation;
		static bool already_created = false;

		const std::lock_guard<std::mutex> lock(avoid_concurrent_creation);

		if (already_created)
			throw std::runtime_error("UI already created.");
			
		already_created = true;
		//return std::make_unique... no, can't see constructor.
		return std::unique_ptr<UserInterface>(new UserInterface());
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
	
	void UserInterface::poll_input(Player& player)
	{
		SDL_Event user_input;
		while (SDL_PollEvent(&user_input) != 0) {
			if (user_input.type == SDL_QUIT)
				halt_game_loop = true;

			else if (user_input.type == SDL_KEYDOWN)
			{
				switch (user_input.key.keysym.sym)
				{
				case SDLK_UP:
					player.advance(1);
					break;
				case SDLK_DOWN:
					player.advance(-1);
					break;
				case SDLK_LEFT:
					player.turn(-1);  // TODO Cross check this. The angles don't match...
					break;
				case SDLK_RIGHT:
					player.turn(+1);
					break;
				}
			}
		}

	}

	void UserInterface::game_loop(const rc::Grid& world)
	{
		halt_game_loop = false;

		ProjectionPlane projection(UserInterface::SCREEN_WIDTH, UserInterface::SCREEN_HEIGHT, 60);
		Player player{ 32 +128, 32 + 128, 2.63};

		SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);  // Background color.
		
		while (! halt_game_loop) {
			poll_input(player);

			if (halt_game_loop)
				return;

			SDL_SetRenderDrawColor(renderer, 120, 120, 120, 255);
			SDL_RenderClear(renderer);

			SDL_SetRenderDrawColor(renderer, 80, 80, 80, 255);
			SDL_Rect half_screen;
			half_screen.x = 0;
			half_screen.y = 0;
			half_screen.h = UserInterface::SCREEN_HEIGHT / 2;
			half_screen.w = UserInterface::SCREEN_WIDTH;
			SDL_RenderFillRect(renderer, &half_screen);

			projection.project_walls(world, player, *this);
			SDL_RenderPresent(renderer);
		}
	}

	void UserInterface::set_wall_texture(const std::string& file_path)
	{
		wall_texture = std::make_unique<Image>(file_path, renderer);
	}

	void UserInterface::draw_wall_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset)
	{
		SDL_Rect source_slice;
		source_slice.x = texture_offset;
		source_slice.y = 0;
		source_slice.w = 1;
		source_slice.h = 64;

		SDL_Rect dest_slice;
		dest_slice.x = column;
		dest_slice.y = top_row;
		dest_slice.w = 1;
		dest_slice.h = height;

		SDL_RenderCopy(renderer, wall_texture->texture, &source_slice, &dest_slice);
	}


}