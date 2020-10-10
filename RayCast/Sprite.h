#pragma once

#include <cstdint>

#include "Grid.h"  // TODO: isolate Ray and RayHit in their own file.
 
namespace rc {

	/** Class to keep track of objects in the map (that are not walls).
		Sprites are assumed to be static and to be projected billboard-stile, as if
		the images are always perpendicular to the viewer.
	*/
	class Sprite
	{
	public:
		/** Create a sprite "size" wide centered in the given position.
			The size should match the texture size. */
		Sprite(const float x_position, const float z_position, const uint8_t size);

		/** Returns a hit that tells if the ray falls inside the sprite. 
		    Uses a simplified intersection formula because we can assume that the sprite is 
			always facing the ray. */
		RayHit intersection(const Ray& ray) const;

		/** Must be public to know how to scale the projection. */
		const uint8_t size;
	private:
		const float x;
		const float z;

	};


	/** Just a collection to keep track of all the enemies. */
	class Enemies {
	public:
		std::vector<Sprite> sprites;
	};
}
