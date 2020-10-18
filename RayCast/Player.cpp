#include "pch.h"
#include "Player.h"

#include <cmath>

#include "PI.h"


namespace rc {
	
	void Player::advance(const float axis, const Grid& map) noexcept
	{
		constexpr float advance_speed = 5.0f;   /// Distance units per key press.

		const float z_step = std::sin(orientation) * advance_speed * axis;
		const float x_step = std::cos(orientation) * advance_speed * axis;

		const float future_position_x = x_position + x_step;
		const float future_position_z = z_position + z_step;

		const GridCoordinate future_cell = map.grid_coordinate(future_position_x, future_position_z);

		if (map.wall_at(future_cell.x, future_cell.z))
			return; // Went inside wall.

		const float forbidden_left = future_cell.x * map.cell_size;
		const float forbidden_right = forbidden_left + map.cell_size;
		const float forbidden_bottom = future_cell.z * map.cell_size;
		const float forbidden_top = forbidden_bottom + map.cell_size;

		constexpr float forbidden_strip_size = 2;

		const bool in_left_strip = std::abs(future_position_x - forbidden_left) < forbidden_strip_size;
		const bool in_right_strip = std::abs(future_position_x - forbidden_right) < forbidden_strip_size;
		const bool in_top_strip = std::abs(future_position_z - forbidden_top) < forbidden_strip_size;
		const bool in_bottom_strip = std::abs(future_position_z - forbidden_bottom) < forbidden_strip_size;

		if (in_left_strip && map.wall_at(future_position_x - 1, future_position_z))
			return;
		if (in_right_strip && map.wall_at(future_position_x + 1, future_position_z))
			return;
		if (in_bottom_strip && map.wall_at(future_position_x, future_position_z - 1))
			return;
		if (in_top_strip && map.wall_at(future_position_x, future_position_z + 1))
			return;
		if (in_left_strip && in_bottom_strip && map.wall_at(future_position_x - 1, future_position_z - 1))
			return;
		if (in_left_strip && in_top_strip && map.wall_at(future_position_x - 1, future_position_z + 1))
			return;
		if (in_right_strip && in_bottom_strip && map.wall_at(future_position_x + 1, future_position_z - 1))
			return;
		if (in_right_strip && in_top_strip && map.wall_at(future_position_x + 1, future_position_z + 1))
			return;


		z_position = future_position_z;
		x_position = future_position_x;
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