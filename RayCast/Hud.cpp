#include "pch.h"
#include "Hud.h"

#include <iomanip>
#include <sstream>

#include "Player.h"

namespace rc {
	void Hud::display(const Player& player, Canvas& c) const
	{
		std::stringstream onscreen_message;

		// TODO: sprintf may be simpler...
		onscreen_message << "AMMO:" << std::setfill('0') << std::setw(2) << (int) player.bullets_left;
		onscreen_message << "  TARGETS:" << std::setw(3) << (int)player.kills;

		c.draw_image((640-300)/2, 480 - 124, TextureIndex::HUD);  // TODO Some kind of "automatically to bottom/center" functionality needed.
		c.draw_text(onscreen_message.str(), 10, 10, 16); // TODO: position/size should depend on screen size, not be hardcoded.
	}

	void Hud::alert_pause(Canvas& c) const
	{
		c.draw_text("PAUSED", 50, 200, 96);  // TODO: again hardcodes...
	}

	void Hud::alert_endgame(Canvas& c) const
	{
		c.draw_text("ESCAPED!", 10, 200, 80);  // TODO: again hardcodes...
		// TODO: semi invisible... need to give a white edge to the letters, to increase contrast when on dark backgrounds.
	}
}
