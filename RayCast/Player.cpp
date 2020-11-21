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


		// Just a ray cast is not enough to see if the movement hits a wall.
		// I want to keep the player at a given distance from the walls. TODO: see if the projection can be improved.
		// (so that the texture distorsion at short distance is not evident).
		// I would have to cast 8 rays - it seem cheaper and simpler to just test the cells near the
		// arrival position.
		const float minimum_wall_distance = map.cell_size / 16;
		if (map.close_to_walls(future_position_x, future_position_z, minimum_wall_distance))
			return;

		z_position = future_position_z;
		x_position = future_position_x;
	}

	void Player::turn(const float axis) noexcept
	{
		constexpr float turn_speed = 0.045f;   /// Circa degrees per frame.

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

	void Player::shoot(const Grid& map, Objects& targets, const Canvas& image_tester) noexcept
	{
		if (bullets_left == 0)
			return;

		const Ray gun_ray(x_position, z_position, orientation);
		const RayHit max_range = map.cast_ray(gun_ray);

		const std::vector<RayHit> hits_targets = targets.all_intersections(gun_ray, max_range, (uint8_t)TextureIndex::ENEMY);

		for (const RayHit& hit : hits_targets) {
			if (!image_tester.transparent_pixel(hit.offset, 32, TextureIndex::ENEMY)) {  // TODO: remove gun height hardcode. Also, is this "in tune" with the projection? Do I have to do something like WC -> local coordinates change? And what if I introduce different enemies?
				targets.deactivate(hit.hit_object_id);
				++kills;  // Optimistically assume we never overflow, there are not that many targets.
			}
		}

		--bullets_left;
	}

}