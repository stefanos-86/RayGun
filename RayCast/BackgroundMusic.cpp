#include "pch.h"
#include "BackgroundMusic.h"

#include <stdexcept>

namespace rc {
	void BackgroundMusic::play_more_music(Loudspeaker& speaker, const Objects& objects, const Player& player)
	{
		if (speaker.still_playing())
			return;

		const float distance_to_exit = objects.distance_to_closest_exit(player.x_position, player.z_position);
		SoundIndex next_segment = SoundIndex::MUSIC_CALM;
		if (distance_to_exit < 64 * 2)  // TODO: do not hardcode cell size
			next_segment = SoundIndex::MUSIC_FAST;
		else if (distance_to_exit < 64  * 5 || player.bullets_left == 0) // You are NOT calm if out of bullets! Pump the music!
			next_segment = SoundIndex::MUSIC_MID;
		else if (distance_to_exit < 64 * 8)
			next_segment = SoundIndex::MUSIC_SLOW;

		speaker.play_this_next(next_segment);
	}
}