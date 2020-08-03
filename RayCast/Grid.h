#pragma once

#include <cstdint>
#include <limits>
#include <unordered_map>

namespace rc {

	/** Coordinates of a "cube of world" in the world grid.
	I have an habit of using Y for the vertical axis. Since the grid is an horizontal plane,
	the coordinates are X and Z.

	I am not sure it was a good idea to use Z instead of Y (like the rest of the world would
	probably do).
	*/
	struct GridCoordinate {
		uint8_t x;
		uint8_t z;

		static constexpr uint8_t OUTSIDE = std::numeric_limits<uint8_t>::max();   /// NO DEFENSE in case the grid is big enough to require this value.
		bool outside_world() const noexcept;
	};


	/** A ray that can be used for ray casting. Graphically:
	   Z
	   ^      _
	   |      /|
	   |     /  
	   |    /alpha
	   |   *----
	   |  (x,z)
	   |   
	    ---------------> X
	*/
	struct Ray {
		Ray(const float x, const float z, const float alpha_rad);
		float x;
		float z;
		float alpha_rad;
	};


	/** Represent the hit of a ray on a wall, carrying the data to the rest of the system.
	It remembers the cell where the wall is, the exact coordinates of the hit, the distance
	from the ray origin and the offset along the cell side of the hit (useful for texturing).
	*/
	struct RayHit {
		GridCoordinate cell;
		float x;
		float z;
		float distance;
		uint8_t offset;
	};

	/** Representation of the world.
	A checkerboard whose cells can be empty or be filled by a wall.
	
	Since the ray casting is all about finding the walls in the grid, this class
	hosts the ray casting algorithm (for a single ray) as well.

	The ray casting logic is not documented on purpose. Read the referenced tutorial for details.
	*/
	class Grid
	{
	public:
		/**Y is the vertical axis. The grid is on the floor, covers X and Z. */
		Grid(uint8_t x_size, uint8_t z_size, uint8_t cell_size);
		
		void build_wall(uint8_t x, uint8_t z);
		bool wall_at(uint8_t x, uint8_t z) const;

		GridCoordinate grid_coordinate(const float x, const float z) const;
		RayHit cast_ray(const Ray& r) const;

		const uint8_t x_size;
		const uint8_t z_size;
		const uint8_t cell_size;

		/** This may not be optimal (in space and time), but sure it is a practical
		structure to represent a grid of empty/full cells without bothering with the 
		dimensions and the search methods. */ 
		std::unordered_map< uint8_t, std::unordered_map < uint8_t, bool> > walls;

	private:
		const float max_x;  /// Useful to determine if an object is inside the grid. Cached at construction time.
		const float max_z;

		RayHit cast_ray_horizontal(const Ray& r, const float tangent) const;
		RayHit cast_ray_vertical(const Ray& r, const float tangent) const;
		RayHit walk_along_ray(const Ray& r,
								       float candidate_point_x,
									   float candidate_point_z,
									   const float horizontal_step,
									   const float vertical_step) const; 

		bool facing_up(const Ray& r) const;
		bool facing_right(const Ray& r) const;

		float distance(const float x1, const float z1, const float x2, const float z2) const;
	};
}

