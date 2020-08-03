#pragma once

#include<cstdint>

namespace rc {

	class Canvas {
	public:
		virtual ~Canvas() {};
		virtual void draw_wall_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset) = 0;

	};
}