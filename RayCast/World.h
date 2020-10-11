#pragma once

#include "Grid.h"
#include "Player.h"
#include "Sprite.h"

namespace rc {

	/** Container of all the objects, to keep things tidy. */
	struct World {
		Grid map;
		Player player;
		Enemies enemies;
	};

}