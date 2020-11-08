#include "pch.h"

#include "Hud.h"

#include "MockCanvas.h"
#include "Player.h"

namespace rc {

    TEST(Hud, display__ammo) {
        Hud h;

        MockCanvas c;
        Player p{0, 0, 0, 25};

        h.display(p, c);

        ASSERT_EQ("AMMO:25", c.last_drawn_string);
    }

    TEST(Hud, display__ammo_single_digit) {
        Hud h;

        MockCanvas c;
        Player p{ 0, 0, 0, 1};

        h.display(p, c);

        ASSERT_EQ("AMMO: 1", c.last_drawn_string);
    }
}