#pragma once

#include <cstdint>
#include <string>

namespace rc {

	enum class TextureIndex {
		WALL,
		ENEMY,
		FONT
	};


	/** Interface that hides the real graphics API and allows to replace it with a mock. 
	
	Not worried about the indirection performance cost. Testing is more important.
	If profiling shows this is a bottleneck, sub-classing can be replaced by templates.
	*/
	class Canvas {
	public:
		virtual ~Canvas() {};

		/** This is the only primitive operation required to draw any object. 
		
		The slice is a 1-pixel-wide strip, to be drawn at the given column of the window.
		The strip starts at the top_row and goes down for all its height. The content is whatever is in the
		1-pixel-wide column of the wall texture at the given offset.

		In other words: take a column of pixels from the texture and copy it in the column of the screen between the 
		top row and the top row+height (scale it if necessary).

		Notice that the top row may be negative, the slice may start outside the screen when the player is very close
		to a the object.
		*/
		virtual void draw_slice(const uint16_t column, const int16_t top_row, const uint16_t height, const uint16_t texture_offset, const TextureIndex what_to_draw) = 0;
	
		/** Returns true if the given pixel of the matching image has a low alpha (less than 128, half-way in the scale).
		* 
		*  Such pixels are invisible to the user and should not be taken into account for collision detection, shooting...
		*/
		virtual bool transparent_pixel(const uint8_t x, const uint8_t y, const TextureIndex image) const = 0;
	
		/** Print the text string starting at the (row, column) pixel. The letters should be square, are assumed to 
		be 8x8 pixels in a bitmap (refer to the file itself to see where the letters go). Scales the letters so that
		they are font_size wide (and tall) pixels on the screen.*/
		virtual void draw_text(const std::string& text, uint16_t column, const uint16_t row, const uint8_t font_size) = 0;

	};
}