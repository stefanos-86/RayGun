#include "pch.h"
#include "BackgroundMusic.h"

#include <stdexcept>

namespace rc {
	void BackgroundMusic::play_more_music(Loudspeaker& speaker, const Objects& objects, const Player& player)
	{
		// TODO: too slow to "change tracks".
		if (speaker.still_playing())
			return;

		// TODO: no visibility check, just distance! They may be behind walls!!!
		const uint8_t enemies_close_to_player = objects.enemies_near(player.x_position, player.z_position, 64 * 5);

		SoundIndex next_segment = SoundIndex::MUSIC_CALM;
		if (enemies_close_to_player > 3)
			next_segment = SoundIndex::MUSIC_FAST;
		else if (enemies_close_to_player > 2 || player.bullets_left == 0) // You are NOT calm if out of bullets!
			next_segment = SoundIndex::MUSIC_MID;
		else if (enemies_close_to_player > 0)
			next_segment = SoundIndex::MUSIC_SLOW;

		speaker.play_this_next(next_segment);
	}
}