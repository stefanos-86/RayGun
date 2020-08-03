#include "pch.h"
#include "ProjectionPlane.h"

#include <cmath>

#include "PI.h"

namespace rc {

	ProjectionPlane::ProjectionPlane(uint16_t h_resolution, uint16_t v_resolution, float FOV_degrees) :
		columns(h_resolution), // Could it be optimized away using x_center?
		x_center(h_resolution / 2), // Round down if odd... maybe emit a warning?
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

	void ProjectionPlane::project_walls(const Grid& grid, const Player& player, Canvas& c) const
	{
		const float original_ray_orientation = player.orientation;

		Ray r{player.x_position, player.z_position, player.orientation};
		r.alpha_rad -= columns / 2 * scan_step_radians; // "Go left" to the beginning.
		
		for (uint16_t scan_column = 0; scan_column < columns; ++scan_column) {
			r.alpha_rad = normalize_0_2pi(r.alpha_rad);
			RayHit hit = grid.cast_ray(r);

			if (hit.cell.x != GridCoordinate::OUTSIDE && hit.cell.z != GridCoordinate::OUTSIDE) {
				
				// Fishbowl.
				const float beta = original_ray_orientation - r.alpha_rad;
				hit.distance *= std::cos(beta);
				
				WallSliceProjection wall_projection = project_wall_slice(hit.distance, grid.cell_size);
				c.draw_wall_slice(scan_column, wall_projection.top_row, wall_projection.height, hit.offset);
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
		const float FOV_radians = to_radians(FOV_degrees);  // TODO: what if FOV <= 0 or PI/2? 
		const float distance = (h_resolution / 2) / (std::tan(FOV_radians / 2));

		return (uint16_t)std::floor(distance);  // TODO: this will overflow, sooner or later.
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