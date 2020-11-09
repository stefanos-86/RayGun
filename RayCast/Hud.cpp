#include "pch.h"
#include "Hud.h"

#include <iomanip>
#include <sstream>

#include "Player.h"

namespace rc {
	void Hud::display(const Player& player, Canvas& c) const
	{
		std::stringstream onscreen_message;

		onscreen_message << "AMMO:" << std::setfill('0') << std::setw(2) << (int) player.bullets_left;
		onscreen_message << "  KILLS:" << std::setw(3) << (int)player.kills;

		c.draw_text(onscreen_message.str(), 10, 10, 16); // TODO: position/size should depend on screen size, not be hardcoded.
	}
	void Hud::alert_pause(Canvas& c) const
	{
		c.draw_text("PAUSED", 50, 200, 96);  // TODO: again hardcodes...
	}
}
