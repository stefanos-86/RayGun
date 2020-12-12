#pragma once

#include <cstdint>

namespace rc {

	enum class SoundIndex : uint8_t {
		GUN_SHOT,
		MUSIC_CALM,
		MUSIC_SLOW,
		MUSIC_MID,
		MUSIC_FAST
	};

	class Loudspeaker {
	public:
		virtual ~Loudspeaker() {};

		/** Returns true if there is music in the buffer. */
		virtual bool still_playing() const noexcept = 0;

		/** Adds the sound segment to the "playlist" for the background music. */
		virtual void play_this_next(const SoundIndex segment) noexcept = 0;
	};

}