#include "pch.h"
#include "Grid.h"

#include <cassert>
#include <cmath>
#include <limits>

#include "PI.h"

namespace rc {
	
	Grid::Grid(uint8_t x_size, uint8_t z_size, uint8_t cell_size):
	x_size(x_size),
	z_size(z_size),
	cell_size(cell_size),
	max_x((float) x_size * cell_size),
	max_z((float) z_size * cell_size)
{
}

void Grid::build_wall(uint8_t x, uint8_t z) noexcept
{
	walls[x][z] = true;
}

bool Grid::wall_at(uint8_t x, uint8_t z) const noexcept
{
	const auto row = walls.find(x);
	if (row == walls.end())
		return false;

	const auto cell = row->second.find(z);
	if (cell == row->second.end())
		return false;

	return cell->second;
}

GridCoordinate Grid::cell_of(const float x, const float z) const noexcept
{
	GridCoordinate result;
	
	if (x >= 0 && x < max_x)
		result.x = (uint8_t)std::floor(x / cell_size);
		
	if (z >= 0 && z < max_z)
		result.z = (uint8_t)std::floor(z / cell_size);

	return result;
}

WorldCoordinate Grid::center_of(uint8_t x, uint8_t z) const noexcept
{
	// It makes sense to give coordinates outside the grid, but it should never happen in practice.
	assert(x < x_size);
	assert(z < z_size);

	const float half_cell = (float) cell_size / 2;
	return WorldCoordinate{ x * cell_size + half_cell, z * cell_size + half_cell};
}



bool Grid::close_to_walls(const float x, const float z, const float minimum_distance) const noexcept
{
	const GridCoordinate cell = cell_of(x, z);

	if (wall_at(cell.x, cell.z))
		return true; // Inside wall: certainly too close.

	// Have to see if there are walls in the cells nearby.
	// Find the "strips" around the grid border that separates from the surrounding cells.
	// If the abutting cells have a wall and the point is in the strip, then it is too close
	// (either inside the abutting cells, or in "this" cell, but too close).
	const float forbidden_left = cell.x * cell_size;
	const float forbidden_right = forbidden_left + cell_size;
	const float forbidden_bottom = cell.z * cell_size;
	const float forbidden_top = forbidden_bottom + cell_size;

	const bool in_left_strip = std::abs(x - forbidden_left) < minimum_distance;
	const bool in_right_strip = std::abs(x - forbidden_right) < minimum_distance;
	const bool in_top_strip = std::abs(z - forbidden_top) < minimum_distance;
	const bool in_bottom_strip = std::abs(z - forbidden_bottom) < minimum_distance;

	return
		in_left_strip && wall_at(cell.x - 1, cell.z)
		|| in_right_strip && wall_at(cell.x + 1, cell.z)
		|| in_bottom_strip && wall_at(cell.x, cell.z - 1)
		|| in_top_strip && wall_at(cell.x, cell.z + 1)
		|| in_left_strip && in_bottom_strip && wall_at(cell.x - 1, cell.z - 1)
		|| in_left_strip && in_top_strip && wall_at(cell.x - 1, cell.z + 1)
		|| in_right_strip && in_bottom_strip && wall_at(cell.x + 1, cell.z - 1)
		|| in_right_strip && in_top_strip && wall_at(cell.x + 1, cell.z + 1)
		;
}

RayHit Grid::cast_ray(const Ray& r) const
{
	const float tangent = std::abs(std::tan(r.alpha_rad));

	RayHit horizontal_hit = cast_ray_horizontal(r, tangent);
	RayHit vertical_hit = cast_ray_vertical(r, tangent);

	// Texture mapping. The hit is defined in walk_along_ray, which does not know
	// if it is looking for vertical or horizontal hits.
	horizontal_hit.offset = (int) horizontal_hit.x % cell_size;  // This cast is a bit brutal, but works decently.
	vertical_hit.offset = (int) vertical_hit.z % cell_size;

	// Select the closest point.
	// A trick could be to assume that the ray will always hit (e. g. by adding wall
	// in the external perimenter of every level). No more need to test if hit is outside the world.
	if (horizontal_hit.no_hit())
		return vertical_hit;  // Either vertical hit, either they are both out and I can return any.
	else
	{
		if (vertical_hit.no_hit())
			return horizontal_hit;
		else // Both hit, test distance.
			return (vertical_hit.distance < horizontal_hit.distance ? vertical_hit : horizontal_hit);
	}
}

RayHit Grid::cast_ray_horizontal(const Ray& r, const float tangent) const
{
	const GridCoordinate starting_cell = cell_of(r.x, r.z);
	const bool ray_goes_up = r.facing_up();
	const bool ray_goes_right = r.facing_right();
	float push_into_lower_row = ray_goes_up ? 0 : -0.0001f;  // TODO: ther's got to be a less kludgy way of doing this. When going towards the negative direction, we have to hit the wall in the cell "lower" coordinate. This fudges it by going into the "higher" of the previous cell...

	float first_point_z = ray_goes_up ?
		(starting_cell.z + 1) * cell_size : // +1 * cell size = top limit of starting cell/bottom of the row above.
		starting_cell.z * cell_size + push_into_lower_row;  // Goes down to the bottom of the starting row.

	const float distance_from_next_row = std::abs(r.z - first_point_z);
	float horizontal_travel_between_rows = distance_from_next_row / tangent;

	float first_point_x = ray_goes_right ?
		r.x + horizontal_travel_between_rows :
		r.x - horizontal_travel_between_rows;

	const float vertical_step = (float)(ray_goes_up ? cell_size : -cell_size);
	const float horizontal_step = ray_goes_right ?
		cell_size / tangent : 
		- cell_size / tangent;

	RayHit hit = walk_along_ray(r, first_point_x, first_point_z,
		horizontal_step, vertical_step);
	hit.z -= push_into_lower_row;
	return hit;
}

RayHit Grid::cast_ray_vertical(const Ray& r, const float tangent) const
{
	const GridCoordinate starting_cell = cell_of(r.x, r.z);
	const bool ray_goes_up = r.facing_up();
	const bool ray_goes_right = r.facing_right(); 
	float push_into_previous_column = ray_goes_right ? 0 : -0.0001f;

	float first_point_x = ray_goes_right ?
		(starting_cell.x + 1) * cell_size :
		starting_cell.x * cell_size + push_into_previous_column;

	const float distance_from_next_column = std::abs(r.x - first_point_x);
	const float distance_from_next_row = distance_from_next_column * tangent;

	float first_point_z = ray_goes_up ?
		r.z + distance_from_next_row :
		r.z - distance_from_next_row;

	const float horizontal_step = (float)(ray_goes_right ? cell_size : -cell_size);

	const float vertical_step = ray_goes_up ?
		cell_size * tangent :
		-cell_size * tangent;

	RayHit hit = walk_along_ray(r, first_point_x, first_point_z,
		horizontal_step, vertical_step);
	hit.x -= push_into_previous_column;
	return hit;

}


/** In case anyone asks, this function is the bottleneck... of the custom code.
Profiling tells that the "heavy lifting" is done by the SDL library.
The hottest line here are 161 and 175, with a scant 6% of the samples each.
...how many of you had a bet on the trig functions or the square roots?
*/
RayHit Grid::walk_along_ray(const Ray& r,
	                                 float candidate_point_x,
									 float candidate_point_z, 
									 const float horizontal_step,
	                                 const float vertical_step) const
{
	RayHit result;  // This is a no hit, by default.

	GridCoordinate candidate_point_cell = cell_of(candidate_point_x, candidate_point_z);
	while (! candidate_point_cell.outside_world())
	{
		if (wall_at(candidate_point_cell.x, candidate_point_cell.z)) {
			result.x = candidate_point_x;
			result.z = candidate_point_z;
			result.distance = distance(candidate_point_x, candidate_point_z, r.x, r.z);
			return result;
		}

		candidate_point_x += horizontal_step;
		candidate_point_z += vertical_step;

		candidate_point_cell = cell_of(candidate_point_x, candidate_point_z);
	}

	return result; // Outside.
}


/** Pithagora's theorem style. TODO: also used for sprite hit. Bring in some header (with PI?)*/
float Grid::distance(const float x1, const float z1, const float x2, const float z2) const
{
	const float side1 = x1 - x2;
	const float side2 = z1 - z2;

	return std::sqrt(side1 * side1 + side2 * side2);  // To micro optimize, use squared distance, do the sqrt only on the closest point. But would it make a difference?
}


GridCoordinate::GridCoordinate() :
	x(GridCoordinate::OUTSIDE),
	z(GridCoordinate::OUTSIDE)
{
}

bool GridCoordinate::outside_world() const noexcept
{
	return (x == GridCoordinate::OUTSIDE || z == GridCoordinate::OUTSIDE);
}

bool operator==(const GridCoordinate& lhs, const GridCoordinate& rhs) noexcept
{
	return lhs.x == rhs.x && lhs.z == rhs.z;
}

bool operator!=(const GridCoordinate& lhs, const GridCoordinate& rhs) noexcept
{
	return ! (lhs == rhs);
}

}
