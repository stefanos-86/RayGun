#include "pch.h"
#include "BackgroundMusic.h"

#include <stdexcept>

namespace rc {
	void BackgroundMusic::play_more_music(Loudspeaker& speaker)
	{
		if (speaker.still_playing())
			return;

		++segment_counter;
		const uint8_t next_segment_index = segment_counter % 4;

		SoundIndex next_segment = SoundIndex::MUSIC_CALM;
		switch (next_segment_index)
		{
		case 0: next_segment = SoundIndex::MUSIC_CALM; break;
		case 1: next_segment = SoundIndex::MUSIC_SLOW; break;
		case 2: next_segment = SoundIndex::MUSIC_MID; break;
		case 3: next_segment = SoundIndex::MUSIC_FAST; break;
		default:
			throw std::runtime_error("math is broken!");
		}

		speaker.play_this_next(next_segment);
	}
}