#include "pch.h"

#include "Ray.h"

namespace rc {

    TEST(Ray, construction) {
        Ray r(1, 2, 3);

        ASSERT_EQ(1, r.x);
        ASSERT_EQ(2, r.z);
        ASSERT_EQ(3, r.alpha_rad);
    }

    TEST(RayHit, construction) {
        RayHit rh;

        ASSERT_TRUE(rh.no_hit());
        ASSERT_FALSE(rh.really_hit());
    }

    TEST(RayHit, record_hit) {
        RayHit rh;
        rh.distance = 5;

        ASSERT_FALSE(rh.no_hit());
        ASSERT_TRUE(rh.really_hit());
    }

}