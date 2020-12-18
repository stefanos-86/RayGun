#pragma once

#include <cstdint>

namespace rc {

	enum class SoundIndex : uint8_t {
		SILENCE,
		GUN_SHOT,
		MUSIC_CALM,
		MUSIC_SLOW,
		MUSIC_MID,
		MUSIC_FAST
	};

	class Loudspeaker {
	public:
		virtual ~Loudspeaker() {};
		
		/** Plays the sound immediately and from the beginning.
			It is OK to "cut" any other sound that was playing before, but leave the music
			to play in the background.
		*/
		virtual void play_sound(const SoundIndex sound) = 0;
	};

}