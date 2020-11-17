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
		Sprite(const float x_position, const float z_position, const uint8_t size, const uint8_t id);

		/** Returns a hit that tells if the ray falls inside the sprite. 
		    Uses a simplified intersection formula because we can assume that the sprite is 
			always facing the ray. */
		RayHit intersection(const Ray& ray) const;

		/** Must be public to know how to scale the projection. */
		const uint8_t size;

		const uint8_t id;  /// Needed to handle hit-scan collisions.

		/** At a certain point, the sprites can be shot and have to be removed from the
		    game. But then I have to make the member non const or somehow allow the vector to
			erase them. I take the coward's shortcut. */ 
		bool active;

	private:
		const float x;
		const float z;
	};


	/** Just a collection to keep track of all the objects that can be seen onscreen. */
	class Objects {
	public:
		std::vector<Sprite> enemies;
		std::vector<Sprite> landmarks;

		enum KIND
		{
			ENEMIES = 0x1,
			LANDMARKS = 0x10
		};

		/** Returns all the hists from the intersection between the ray and any of the sprites.
		    Hits further from the ray than the cutoff distance (the distance of the cutoff hit) are discarded.
			Hits are sorted by distance in reverse (the more distant first) to help over-paint them.

			No broad-phase in the ray-sprite collision detection.
			TODO: find a way to avoid testing every sprite.

			There is also room to improve the memory managment, probably (e. g. do not return a new vector every time).
		*/
		std::vector <RayHit> all_intersections(const Ray& ray, const RayHit& cutoff, const int enumerated_kinds) const noexcept;

		void deactivate(const uint8_t sprite_id);
	};
}
