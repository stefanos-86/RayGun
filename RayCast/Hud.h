#pragma once

namespace rc {
	class Player;
	class Canvas;

	class Hud
	{
	public:
		void display(const Player& player, Canvas& c) const;
	};
}

