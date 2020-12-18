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
		/** Decides what to send to the loudspeaker next.
		* 
		* The current logic gives an audio queue to the distance of the closest exit (disregarding walls).
		* (and keeps an "excited" music when out of ammo).
		* 
		* It is somewhat basic and not really effective.
		TODO: test.
		*/
		SoundIndex select_music_score(const Objects& objects, const Player& player);


	private:
		uint8_t segment_counter;
	};
}

