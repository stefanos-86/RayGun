#pragma once

#include <cstdint>

#include "Grid.h"
#include "Ray.h"
 
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
		// TODO: auto-assign the ID.
		Sprite(const float x_position, const float z_position, const uint8_t size, const uint8_t id, const TextureIndex kind);

		/** Returns a hit that tells if the ray falls inside the sprite. 
		    Uses a simplified intersection formula because we can assume that the sprite is 
			always facing the ray. */
		RayHit intersection(const Ray& ray) const;

		/** Must be public to know how to scale the projection. */
		const uint8_t size;

		const uint8_t id;  /// Needed to handle hit-scan collisions.
		
		/** The Sprite has the extra responsibility of being a game object. Its kind must be 
		    known. Notice that this implied that each object is a texture, the texture and
			the game object are one and the same. 
			
			Not really object oriented, but I have only 2 kinds of game objects with no
			behavior other than being seen, so I can cut the corners.
			*/
		const TextureIndex kind;

		/** At a certain point, the sprites can be shot and have to be removed from the
		    game. But then I have to make the member non const or somehow allow the vector to
			erase them. I take the coward's shortcut. */ 
		bool active;

		const float x;
		const float z;
	};


}
