#pragma once

#include<cstdint>

namespace rc {

	/** Simplifed-to-the-bone class to represent the player character. Position and movement.
		
		Since SDL should take care of the frame rate and v-synch it to the screen refresh rate,
		I can imagine that each game loop has a fixed duration.
		It is reasonable to assume that the framerate won't drop too much below 60 FPS on a modern
		PC, given how simple the code is.

		Therefore, the movement function only take the direction, without the frame duration.

		Player-wall collisions ignored on purpose, not part of this exercise (it would not be too difficult 
		to implement them just using the grid, or reusing the ray cast to find the distance to the nearest wall).

		The position is directly in world coordinates. The orientation is absolute (0 means "toward +X",
		PI is "towards -X"...).
	*/
	class Player
	{
	public:
		void advance(const float axis);
		void turn(const float axis);

		float x_position;
		float z_position;
		float orientation;
	};

}

