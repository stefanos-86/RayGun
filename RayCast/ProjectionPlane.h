#pragma once

#include <cstdint>

#include "World.h"
#include "Canvas.h"



namespace rc {

	/** Results of projecting a piece of some object. */
	struct SliceProjection{
		uint16_t height;  
		int16_t top_row; /// Can be negative if you are really close to the wall and the slice it taller than the screen.
	};


	/** The ProjectionPlane class computes the simplified projection, after the ray casting. 
	    Assumed to have the same resolution as the window on the screen. I did not want to deal with
		viewport scaling.
		
		The math behind the plane geometry assumes "reasonable" values. This class was not coded defensively,
		there may be over/underflows if the Field Of View is very small or wide, if there are few or too many
		columns etcetera.
		*/
	class ProjectionPlane
	{
	public:
		ProjectionPlane(uint16_t h_resolution, uint16_t v_resolution, float FOV_degrees);

		/** "Semi-private" function. It is not used outside the class, but I felt I had to test it 
		to ensure correctness. */
		SliceProjection project_slice(const float hit_distance, const uint16_t cell_size) const;

		/** Entry point of the exercise. Does the ray casting scan.
		
		Simplified explaination: for each column of the screen/projection plane, cast a ray from the player.
		If it hits a wall, project the vertical wall slice in the canvas.

		It's harder than this. Consult the project reference material.
		*/
		void project_objects(const World& grid, Canvas &c) const;

		const uint16_t columns;
		const uint16_t x_center;
		const uint16_t y_center;
		const uint16_t distance_to_POV;
		const float scan_step_radians;

	private:
		uint16_t pov_distance(uint16_t h_resolution, float FOV_degrees) const;
		float to_radians(const float degrees) const;
		float normalize_0_2pi(float radians) const;
	};
}

