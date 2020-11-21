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


	/** Just a collection to keep track of all the objects that can be seen onscreen. */
	class Objects {
	public:
		std::vector<Sprite> enemies;
		std::vector<Sprite> exits;  // TODO: do I want to keep multiple exits? Can I cache the cell they are into? 
										// If you also think of the enemies status, I start to think I need game objects that contains a sprite, rather than just the sprite.
		                                // What does it imply for the ray collisions?


		/** Returns all the hists from the intersection between the ray and any of the sprites.
		    Hits further from the ray than the cutoff distance (the distance of the cutoff hit) are discarded.
			Hits are sorted by distance in reverse (the more distant first) to help over-paint them.

			No broad-phase in the ray-sprite collision detection.
			TODO: find a way to avoid testing every sprite.

			TODO There is also room to improve the memory managment, probably (e. g. do not return a new vector every time).
		*/
		std::vector <RayHit> all_intersections(const Ray& ray, const RayHit& cutoff, const uint8_t enumerated_kinds) const noexcept;

		void deactivate(const uint8_t sprite_id);

	private:
		std::vector <RayHit> intersections(const Ray& ray, const RayHit& cutoff, std::vector<Sprite> objects) const;
	};
}
