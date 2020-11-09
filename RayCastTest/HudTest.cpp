#include "pch.h"

#include "Hud.h"

#include "MockCanvas.h"
#include "Player.h"

namespace rc {

    TEST(Hud, display__ammo_kills) {
        Hud h;

        MockCanvas c;
        Player p{0, 0, 0, 25, 123};

        h.display(p, c);

        ASSERT_EQ("AMMO:25  KILLS:123", c.last_drawn_string);
    }

    TEST(Hud, display__ammo_kills_single_digit) {
        Hud h;

        MockCanvas c;
        Player p{ 0, 0, 0, 1, 0};

        h.display(p, c);

        ASSERT_EQ("AMMO:01  KILLS:000", c.last_drawn_string);
    }
}