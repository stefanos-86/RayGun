#include "pch.h"
#include "ProjectionPlane.h"

#include <cmath>

#include "PI.h"
#include "Sprite.h"

namespace rc {

	ProjectionPlane::ProjectionPlane(uint16_t h_resolution, uint16_t v_resolution, float FOV_degrees) :
		columns(h_resolution),
		x_center(h_resolution / 2),
		y_center(v_resolution / 2),
		distance_to_POV(pov_distance(h_resolution, FOV_degrees)),
		scan_step_radians(to_radians(FOV_degrees / h_resolution))
	{
	}

	WallSliceProjection ProjectionPlane::project_wall_slice(const float hit_distance, const uint16_t cell_size) const
	{
		WallSliceProjection projected_slice;
		projected_slice.height = (uint16_t) (cell_size / hit_distance * distance_to_POV);
		projected_slice.top_row = y_center - projected_slice.height / 2;

		return projected_slice;
	}

	void ProjectionPlane::project_objects(const World& world, Canvas& c) const
	{
		const Grid& grid = world.map;
		const Player& player = world.player;

		const float original_ray_orientation = player.orientation;

		Ray r{player.x_position, player.z_position, player.orientation};
		r.alpha_rad -= columns / 2 * scan_step_radians; // "Go left" to the beginning.
		
		for (uint16_t scan_column = 0; scan_column < columns; ++scan_column) {
			r.alpha_rad = normalize_0_2pi(r.alpha_rad);
			RayHit hit = grid.cast_ray(r);

			if (! hit.cell.outside_world()) {

				// Fishbowl correction.
				const float beta = original_ray_orientation - r.alpha_rad;
				hit.distance *= std::cos(beta);
				
				const WallSliceProjection wall_projection = project_wall_slice(hit.distance, grid.cell_size);
				c.draw_slice(scan_column, wall_projection.top_row, wall_projection.height, hit.offset, TextureIndex::WALL);
			}


			for (const Sprite& testSprite : world.enemies.sprites) {
				RayHit enemy_hit = testSprite.intersection(r);
				if (!enemy_hit.cell.outside_world() &&
					enemy_hit.distance < hit.distance) // Not behind the wall
				{
					// Fishbowl correction AGAIN! Well, it does not seem so evident...
					//const float beta = original_ray_orientation - r.alpha_rad;
					//enemy_hit.distance *= std::cos(beta);

					// Try to see if the same projection works... at least until sprite and cell are the same size.
					const WallSliceProjection enemy_projection = project_wall_slice(enemy_hit.distance, testSprite.size);
					c.draw_slice(scan_column, enemy_projection.top_row, enemy_projection.height, enemy_hit.offset, TextureIndex::ENEMY);

				}
			}

			r.alpha_rad += scan_step_radians;
		}
	}

	/** Consider a triangle with a vertex in the center of the plane, one in the Point of View, the last
		on the vertical border of the plane. Then do trig. Seen from above, it is:

		-----+----+ Projection plane.
			 |   /
			 |  /
			 | /
			 |/
			 +
		   Point of view
		   (Angle here is half the FOV).
		*/
	uint16_t ProjectionPlane::pov_distance(uint16_t h_resolution, float FOV_degrees) const
	{
		const float FOV_radians = to_radians(FOV_degrees);
		const float distance = (h_resolution / 2) / (std::tan(FOV_radians / 2));

		return (uint16_t)std::floor(distance);
	}


	float ProjectionPlane::to_radians(const float degrees) const
	{
		return normalize_0_2pi(degrees * (float)PI / 180);
	}

	float ProjectionPlane::normalize_0_2pi(float radians) const
	{
		while (radians < 0) // TODO: there's got to be a formula...
			radians += 2 * PI;

		return radians - std::floor(radians / (2 * PI)) * radians;
	}

}