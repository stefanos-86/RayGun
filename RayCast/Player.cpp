#include "pch.h"
#include "Player.h"

#include <cmath>

#include "PI.h"

namespace rc {
	
	void Player::advance(const float axis) noexcept
	{
		constexpr float advance_speed = 5.0f;   /// Distance units per key press.

		const float z_step = std::sin(orientation) * advance_speed * axis;
		const float x_step = std::cos(orientation) * advance_speed * axis;

		z_position += z_step;
		x_position += x_step;
	}

	void Player::turn(const float axis) noexcept
	{
		constexpr float turn_speed = 0.025f;   /// Circa 0.25 degrees per key frame.

		orientation += turn_speed * axis;

		// Normalize between 0 and 2 PI, like all the angles in the project.
		// Notice that the clamping is really "severe" (e. g. negative angles
		// go to 2PI, no matter how much behind 0 they are), but in practice
		// it works. You don't see jitter in the game.
		// This is surprising enough - keep this code as a "memento".
		if (orientation < 0)
			orientation = 2 * PI;
		if (orientation > 2 * PI)
			orientation = 0;
	}

}