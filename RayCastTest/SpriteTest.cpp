#include "pch.h"

#include "Sprite.h"

#include "Ray.h"
#include "PI.h"

namespace rc {
    TEST(Sprite, rayIntersection_onTheRight_inFront) {
        const Sprite s(10, 0, 64);
        const Ray goingRight(0, 0, 0);

        const RayHit hit = s.intersection(goingRight);

        ASSERT_EQ(hit.distance, 10);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_onTheRight_behind) {
        const Sprite s(-10, 0, 64);
        const Ray goingRight(0, 0, 0);

        const RayHit hit = s.intersection(goingRight);

        ASSERT_TRUE(hit.no_hit());
    }

    TEST(Sprite, rayIntersection_onTheLeft_inFront) {
        const Sprite s(-10, 0, 64);
        const Ray goingLeft(0, 0, PI);

        const RayHit hit = s.intersection(goingLeft);

        ASSERT_EQ(hit.distance, 10);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_onTheLeft_behind) {
        const Sprite s(10, 0, 64);
        const Ray goingLeft(0, 0, PI);

        const RayHit hit = s.intersection(goingLeft);

        ASSERT_TRUE(hit.no_hit());
    }
    

    TEST(Sprite, rayIntersection_up_above) {
        const Sprite s(0, 10, 64);
        const Ray goUp(0, 0, PI / 2);

        const RayHit hit = s.intersection(goUp);

        ASSERT_EQ(hit.distance, 10);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_up_below) {
        const Sprite s(0, -10, 64);
        const Ray goUp(0, 0, PI / 2);

        const RayHit hit = s.intersection(goUp);

        ASSERT_TRUE(hit.no_hit());
    }

    TEST(Sprite, rayIntersection_down_above) {
        const Sprite s(0, 10, 64);
        const Ray goDown(0, 0, - PI / 2);

        const RayHit hit = s.intersection(goDown);

        ASSERT_TRUE(hit.no_hit());
    }

    TEST(Sprite, rayIntersection_down_below) {
        const Sprite s(0, -10, 64);
        const Ray goDown(0, 0, - PI / 2);

        const RayHit hit = s.intersection(goDown);

        ASSERT_EQ(hit.distance, 10);
        ASSERT_EQ(hit.offset, 32);
    }


    TEST(Sprite, rayIntersection_1tQuadrant) {
        const Sprite s(10, 10, 64);
        const Ray diagonal(0, 0, PI / 4);

        const RayHit hit = s.intersection(diagonal);

        ASSERT_FLOAT_EQ(hit.distance, 14.14213562373095);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_2ndQuadrant) {
        const Sprite s(-10, 10, 64);
        const Ray diagonal(0, 0, 3 * PI / 4);

        const RayHit hit = s.intersection(diagonal);

        ASSERT_FLOAT_EQ(hit.distance, 14.14213562373095);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_3rdQuadrant) {
        const Sprite s(-10, -10, 64);
        const Ray diagonal(0, 0, 5 * PI / 4);

        const RayHit hit = s.intersection(diagonal);

        ASSERT_FLOAT_EQ(hit.distance, 14.14213562373095);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_4thQuadrant) {
        const Sprite s(10, -10, 64);
        const Ray diagonal(0, 0, 7 * PI / 4);

        const RayHit hit = s.intersection(diagonal);

        ASSERT_FLOAT_EQ(hit.distance, 14.14213562373095);
        ASSERT_EQ(hit.offset, 32);
    }

    TEST(Sprite, rayIntersection_outside) {
        const Sprite s(1, 33, 64);
        const Ray goRight(0, 0, 0);

        const RayHit hit = s.intersection(goRight);

        ASSERT_TRUE(hit.no_hit());
    }

    // TODO: test the multi-sprite intersection.
}