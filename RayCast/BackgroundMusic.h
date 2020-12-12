#pragma once

#include "Loudspeaker.h"
#include "Objects.h"
#include "Player.h"

namespace rc {
	// Many thanks to https://gist.github.com/armornick/3447121
		// Instruction for mixing: https://discourse.libsdl.org/t/sdl2-play-multiple-wav-sounds-simultaneously-no-sdl2-mixer/25439/14


	class BackgroundMusic
	{
	public:
		// TODO: test.
		void play_more_music(Loudspeaker& speaker, const Objects& objects, const Player& player);

	private:
		uint8_t segment_counter;
	};
}

