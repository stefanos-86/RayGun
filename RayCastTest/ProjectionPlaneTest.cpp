#include "pch.h"

#include "ProjectionPlane.h"

#include <vector>

#include "Canvas.h"
#include "Grid.h"
#include "Player.h"

namespace rc {

    class MockCanvas : public Canvas {
    public:
        void draw_wall_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset) final {
            column_calls.push_back(column);
            top_row_calls.push_back(top_row);
            height_calls.push_back(height);
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

    TEST(ProjectionPlane, project_wall_slice) {
        ProjectionPlane p(320, 200, 60);
        const WallSliceProjection result = p.project_wall_slice(277, 64);

        ASSERT_EQ(64, result.height);
        ASSERT_EQ(68, result.top_row);
    }


    TEST(ProjectionPlane, project_walls__no_walls) {
        ProjectionPlane plane(320, 200, 60);
        Grid g(10, 10, 64);
        Player p{ 32, 32, 0.5 };
        MockCanvas mc;

        plane.project_walls(g, p, mc);

        ASSERT_TRUE(mc.column_calls.empty());
    }


    TEST(ProjectionPlane, project_walls__two_calls__takes_all_view) {
        ProjectionPlane plane(2, 200, 1);
        Grid g(2, 1, 64);
        g.build_wall(1, 0);
        Player p{ 63, 32, 0 };  // 1 unit in front of the wall.
        MockCanvas mc;

        plane.project_walls(g, p, mc);

        ASSERT_EQ(2, mc.column_calls.size());
        ASSERT_EQ(61988, mc.top_row_calls.at(0));  // Severe underflow?
        ASSERT_EQ(7296, mc.height_calls.at(0));
    }
}