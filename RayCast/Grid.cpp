#include "pch.h"
#include "Grid.h"

#include <cmath>
#include <limits>

#include "PI.h"

namespace rc {
	const uint8_t GridCoordinate::OUTSIDE = std::numeric_limits<uint8_t>::max();  // TODO: ensure other parts of the code knows this value is reserved.

	Ray::Ray(const float x, const float z, const float alpha_rad) :
		x(x),
		z(z),
		alpha_rad(alpha_rad)
	{
	}

Grid::Grid(uint8_t x_size, uint8_t z_size, uint8_t cell_size):
	x_size(x_size),
	z_size(z_size),
	cell_size(cell_size)
{
}

void Grid::build_wall(uint8_t x, uint8_t z)
{
	walls[x][z] = true;
}

bool Grid::wall_at(uint8_t x, uint8_t z) const
{
	const auto row = walls.find(x);
	if (row == walls.end())
		return false;

	const auto cell = row->second.find(z);
	if (cell == row->second.end())
		return false;

	return cell->second;
}

GridCoordinate Grid::grid_coordinate(const float x, const float z) const
{
	// TODO: compute on construction.
	const float max_x = (float) x_size * cell_size;
	const float max_z = (float) z_size * cell_size;

	GridCoordinate result{GridCoordinate::OUTSIDE, GridCoordinate::OUTSIDE };
	
	if (x >= 0 && x < max_x && z >= 0 && z < max_z) {
		result.x = (uint8_t)std::floor(x / cell_size);
		result.z = (uint8_t)std::floor(z / cell_size);
	}

	return result;
}

RayHit Grid::cast_ray(const Ray& r) const
{
	const float tangent = std::abs(std::tan(r.alpha_rad)); // Hoisted up just to micro optimize.
	RayHit horizontal_hit = cast_ray_horizontal(r, tangent);
	RayHit vertical_hit = cast_ray_vertical(r, tangent);

	// Texture mapping.
	horizontal_hit.offset = (int) horizontal_hit.x % cell_size;  // The brutality of this cast...
	vertical_hit.offset = (int)vertical_hit.z % cell_size;

	RayHit candidate = horizontal_hit;  // Sooo many copies to micro optimize!!!
	if (horizontal_hit.cell.outside_world())
		candidate = vertical_hit;
	else if (vertical_hit.cell.outside_world())
		return candidate; // Both out, failure.
	else if (vertical_hit.distance < horizontal_hit.distance)
		candidate = vertical_hit;
	// Else, horizontal is closer (already set).

	return candidate;
}

RayHit Grid::cast_ray_horizontal(const Ray& r, const float tangent) const
{
	const GridCoordinate starting_cell = grid_coordinate(r.x, r.z);
	const bool ray_goes_up = facing_up(r);
	const bool ray_goes_right = facing_right(r);
	float push_into_previous_row = ray_goes_up ? 0 : -1.0f;

	float first_point_z = ray_goes_up ?
		starting_cell.z * cell_size + cell_size :
		starting_cell.z * cell_size + push_into_previous_row;

	const float distance_from_next_row = std::abs(r.z - first_point_z);
	float horizontal_travel_between_rows = distance_from_next_row / tangent;

	float first_point_x = ray_goes_right ?
		r.x + horizontal_travel_between_rows :
		r.x - horizontal_travel_between_rows;

	const float vertical_step = (float)(ray_goes_up ? cell_size : -cell_size);
	const float horizontal_step = ray_goes_right ?
		cell_size / tangent : 
		- cell_size / tangent;

	RayHit hit = cast_ray_walk_along_ray(r, first_point_x, first_point_z,
		horizontal_step, vertical_step);
	hit.z -= push_into_previous_row;
	return hit;
}

RayHit Grid::cast_ray_vertical(const Ray& r, const float tangent) const
{
	const GridCoordinate starting_cell = grid_coordinate(r.x, r.z);
	const bool ray_goes_up = facing_up(r);
	const bool ray_goes_right = facing_right(r); 
	float push_into_previous_column = ray_goes_right ? 0 : -1.0f;

	float first_point_x = ray_goes_right ?
		starting_cell.x * cell_size + cell_size:
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

	RayHit hit = cast_ray_walk_along_ray(r, first_point_x, first_point_z,
		horizontal_step, vertical_step);
	hit.x -= push_into_previous_column;
	return hit;

}

RayHit Grid::cast_ray_walk_along_ray(const Ray& r,
	                                 float candidate_point_x,
									 float candidate_point_z, 
									 const float horizontal_step,
	                                 const float vertical_step) const
{
	RayHit result;
	result.cell.x = GridCoordinate::OUTSIDE;
	result.cell.z = GridCoordinate::OUTSIDE;

	GridCoordinate candidate_point_cell = grid_coordinate(candidate_point_x, candidate_point_z);
	while (! candidate_point_cell.outside_world())
	{
		if (wall_at(candidate_point_cell.x, candidate_point_cell.z)) {
			result.x = candidate_point_x;
			result.z = candidate_point_z;
			result.cell = candidate_point_cell;
			result.distance = distance(candidate_point_x, candidate_point_z, r.x, r.z);
			return result;
		}

		candidate_point_x += horizontal_step;
		candidate_point_z += vertical_step;

		candidate_point_cell = grid_coordinate(candidate_point_x, candidate_point_z);
	}

	return result; // Outside.
}

bool Grid::facing_up(const Ray& r) const
{
	return (0 <= r.alpha_rad && r.alpha_rad < PI);
}

bool Grid::facing_right(const Ray& r) const
{
	return (0 <= r.alpha_rad && r.alpha_rad < PI / 2) || (3 * PI / 2 <= r.alpha_rad && r.alpha_rad < 2 * PI);
}

float Grid::distance(const float x1, const float z1, const float x2, const float z2) const
{
	const float side1 = x1 - x2;
	const float side2 = z1 - z2;

	return std::sqrt(side1 * side1 + side2 * side2);  // To micro optimize, use squared distance, do the sqrt only on the closest point.
}


bool GridCoordinate::outside_world() const noexcept
{
	return (x == GridCoordinate::OUTSIDE || z == GridCoordinate::OUTSIDE);
}

}
