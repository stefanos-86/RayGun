#pragma once

#include<cstdint>
#include<unordered_map>

namespace rc {

	/** Remember the coordinates:
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
	class Ray {
	public: 
		Ray(const float x, const float z, const float alpha_rad);
		float x;
		float z;
		float alpha_rad;
	};

	struct GridCoordinate {
		uint8_t x;
		uint8_t z;

		static const uint8_t OUTSIDE;

		bool outside_world() const noexcept;
	};

	struct RayHit {
		GridCoordinate cell;
		float x;
		float z;
		float distance;
		uint8_t offset;
	};


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

		// This may not be optimal (in space and time), but sure it is practical.
		std::unordered_map< uint8_t, std::unordered_map < uint8_t, bool> > walls;

	private:
		RayHit cast_ray_horizontal(const Ray& r, const float tangent) const;
		RayHit cast_ray_vertical(const Ray& r, const float tangent) const;
		RayHit cast_ray_walk_along_ray(const Ray& r,
								       float candidate_point_x,
									   float candidate_point_z,
									   const float horizontal_step,
									   const float vertical_step) const; 

		bool facing_up(const Ray& r) const;
		bool facing_right(const Ray& r) const;

		float distance(const float x1, const float z1, const float x2, const float z2) const;
	};
}

