#pragma once

#include "BackgroundMusic.h"
#include "Grid.h"
#include "Hud.h"
#include "Objects.h"
#include "Player.h"

namespace rc {

	

	/** Container of all the objects, to keep things tidy. */
	class World {
	public:
		Grid map;
		Player player;
		Objects sprites;
		Hud hud;
		BackgroundMusic music;

		static World load(std::istream& serialized_world);

		/** Tells if the game is complete. True when the player is in the same cell as an exit. */
		bool endgame() const;  // TODO test
	};

}