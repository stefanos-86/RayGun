#include "pch.h"
#include "Player.h"

#include <cmath>

#include "PI.h"

#include <iostream>

namespace rc {
	const float Player::advance_speed = 10;
	const float Player::turn_speed = 0.05f;

	void Player::advance(const float axis)
	{
		const float z_step = std::sin(orientation) * advance_speed * axis;
		const float x_step = std::cos(orientation) * advance_speed * axis;
		// TODO: collisions!  Proabably also prevents going to a negative position (which I think it is not well handled).

		z_position += z_step;
		x_position += x_step;

		std::cout << x_position << ' ' << z_position << ' ' << orientation << std::endl;
	}

	void Player::turn(const float axis)
	{
		orientation += turn_speed * axis; // TODO Must normalize!

		if (orientation < 0)
			orientation = 2 * PI;
		if (orientation > 2 * PI)
			orientation = 0;

		std::cout << x_position << ' ' << z_position << ' ' << orientation << std::endl;
	}

}