//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract superclass for fonts.

#ifndef FONT_H_
#define FONT_H_

#include "include/eyeGUI.h"
#include "Defines.h"

#include "externals/GLM/glm/glm.hpp"

namespace eyegui
{
    struct Glyph
    {
        glm::vec4	atlasPosition;	// Position in atlas (minU, minV, maxU, maxV)
        glm::ivec2  size;			// Size in pixel
        glm::ivec2	bearing;		// Offset from baseline to left / top of glyph in pixel
        glm::vec2	advance;        // Offset to advance to next glyph in pixel
    };

    class Font
    {
    public:

        // Constructor
        Font() {}

        // Destructor
        virtual ~Font() {}

        // Resize font atlases
        virtual void resizeFontAtlases(int windowHeight) = 0;

        // Get glyph (may return NULL if not found)
        virtual Glyph const * getGlyph(FontSize fontSize, char16_t character) const = 0;

        // Get height of line
        virtual float getLineHeight(FontSize fontSize) const = 0;

		// Bind atlas texture of font
		virtual void bindAtlasTexture(FontSize fontSize, uint slot = 0, bool linearFiltering = false) const = 0;
    };
}

#endif // FONT_H_
