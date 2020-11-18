#pragma once

#include "Grid.h"
#include "Hud.h"
#include "Player.h"
#include "Sprite.h"

namespace rc {

	/** Container of all the objects, to keep things tidy. */
	struct World {
		Grid map;
		Player player;
		Objects sprites;
		Hud hud;

		static World load(std::istream& serialized_world);

		/** Tells if the game is complete. True when the player is in the same cell as an exit. */
		bool endgame() const;  // TODO test
	};

}