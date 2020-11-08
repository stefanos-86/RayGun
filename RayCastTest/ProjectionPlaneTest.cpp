#include "pch.h"

#include "ProjectionPlane.h"

#include <vector>

#include "Canvas.h"
#include "Grid.h"
#include "Player.h"
#include "World.h"

namespace rc {

    class MockCanvas : public Canvas {
    public:
        void draw_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset, const TextureIndex what_to_draw) final {
            column_calls.push_back(column);
            top_row_calls.push_back(top_row);
            height_calls.push_back(height);
        }

        bool transparent_pixel(const uint8_t x, const uint8_t y, const TextureIndex image) const final {
            return false;
        }

        void draw_text(const std::string& text, uint16_t row, const int16_t column, const uint8_t font_size) const final {
            // Never used.
        }

        std::vector<uint16_t> column_calls;
        std::vector<uint16_t> top_row_calls;
        std::vector<uint16_t> height_calls;
    };

    TEST(ProjectionPlane, Creation) {
        ProjectionPlane p(320, 200, 60);

        ASSERT_EQ(320, p.columns);
        ASSERT_EQ(160, p.x_center);
        ASSERT_EQ(100, p.y_center);
        ASSERT_EQ(277, p.distance_to_POV);
        ASSERT_FLOAT_EQ(0.0032724924f, p.scan_step_radians);
    }

    TEST(ProjectionPlane, project_slice__wall_slice) {
        ProjectionPlane p(320, 200, 60);
        const SliceProjection result = p.project_slice(277, 64);

        ASSERT_EQ(64, result.height);
        ASSERT_EQ(68, result.top_row);
    }


    TEST(ProjectionPlane, project_slice__walls__no_walls) {
        ProjectionPlane plane(320, 200, 60);
        Grid g(10, 10, 64);
        Player p{ 32, 32, 0.5 };
        World w{ g, p, {{}} };
        MockCanvas mc;


        plane.project_objects(w, mc);

        ASSERT_TRUE(mc.column_calls.empty());
    }


    TEST(ProjectionPlane, project_slice__walls__two_calls__takes_all_view) {
        ProjectionPlane plane(2, 200, 1);
        Grid g(2, 1, 64);
        g.build_wall(1, 0);
        Player p{ 63, 32, 0 };  // 1 unit in front of the wall.
        World w{ g, p, {} };
        MockCanvas mc;

        plane.project_objects(w, mc);

        ASSERT_EQ(2, mc.column_calls.size());
        ASSERT_EQ(61988, mc.top_row_calls.at(0));  // TODO: Severe underflow?
        ASSERT_EQ(7296, mc.height_calls.at(0));
    }

    // TODO: test projection of enemies.
}