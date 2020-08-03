#pragma once

#include <cstdint>

#include "Canvas.h"
#include "Grid.h"
#include "Player.h"

namespace rc {

	struct WallSliceProjection {
		uint16_t height;  
		int16_t top_row;// can go negative if you are really close to the wall and the slice it taller than the screen.
	};

	class ProjectionPlane
	{
	public:
		ProjectionPlane(uint16_t h_resolution, uint16_t v_resolution, float FOV_degrees);

		WallSliceProjection project_wall_slice(const float hit_distance, const uint16_t cell_size) const;
		void project_walls(const Grid& grid, const Player& player, Canvas &c) const;

		const uint16_t columns;
		const uint16_t x_center;
		const uint16_t y_center;
		const uint16_t distance_to_POV;  // TODO: overflow handling?
		const float scan_step_radians;

	private:
		uint16_t pov_distance(uint16_t h_resolution, float FOV_degrees) const;
		float to_radians(const float degrees) const;
		float normalize_0_2pi(float radians) const;
	};
}

