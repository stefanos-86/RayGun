#pragma once

#include "Canvas.h"
#include "Loudspeaker.h"

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

        void draw_text(const std::string& text, uint16_t column, const uint16_t row, const uint8_t font_size) final {
            last_drawn_string = text;
        }

        void draw_image(uint16_t column_x, const uint16_t row_y, const TextureIndex what_to_draw) 
        {
        }

        std::vector<uint16_t> column_calls;
        std::vector<uint16_t> top_row_calls;
        std::vector<uint16_t> height_calls;

        std::string last_drawn_string;
    };

    class MockSpeaker : public Loudspeaker {
    public:
        void play_sound(const SoundIndex sound) {};
    };
}