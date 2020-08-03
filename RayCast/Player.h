#pragma once

#include<cstdint>

namespace rc {

	class Player
	{
	public:
		static const float advance_speed;  // "10 unit per key press" or so.
		static const float turn_speed;  // "2.5 degrees per key press" or so.

		void advance(const float axis); // TODO account for timestamp.
		void turn(const float axis); // TODO account for timestamp.

		float x_position;
		float z_position;
		float orientation;  // TODO: normalize, once there is logic to move the player.
	};

}

