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

	SliceProjection ProjectionPlane::project_slice(const float hit_distance, const uint16_t cell_size) const
	{
		SliceProjection projected_slice;
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

			const float fishbowl = std::cos(original_ray_orientation - r.alpha_rad);

			if (hit.really_hit()) {
				hit.distance *= fishbowl;
				
				const SliceProjection wall_projection = project_slice(hit.distance, grid.cell_size);
				c.draw_slice(scan_column, wall_projection.top_row, wall_projection.height, hit.offset, TextureIndex::WALL);
			}


			for (const Sprite& testSprite : world.enemies.sprites) {  //TODO: use an intersection method at collection level to account for z-order.
				RayHit enemy_hit = testSprite.intersection(r);
				if (enemy_hit.really_hit() &&  // Don't bother if there's not it.
   				   (hit.no_hit() || enemy_hit.distance < hit.distance))  // Depth test: either no wall, either in front of it.
				{
					enemy_hit.distance *= fishbowl;

					// TODO Try to see if the same projection works... at least until sprite and cell are the same size.
					const SliceProjection enemy_projection = project_slice(enemy_hit.distance, testSprite.size);
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