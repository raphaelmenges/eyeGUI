//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Implementation of font with nothing to display. Used as fallback when user
// specifies no or wrong path to font file and default font ist needed.

#ifndef EMPTY_FONT_H_
#define EMPTY_FONT_H_

#include "Font.h"

#include "externals/GLM/glm/glm.hpp"
#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    class EmptyFont : public Font
    {
    public:

        // Constructor
        EmptyFont();

        // Destructor
        virtual ~EmptyFont();

        // Resize font atlases
        virtual void resizeFontAtlases();

        // Get glyph (may return NULL if not found)
        virtual Glyph const * getGlyph(FontSize fontSize, char16_t character) const;

        // Get height of line
        virtual float getLineHeight(FontSize fontSize) const;

        // Get target glyph height
        virtual float getTargetGlyphHeight(FontSize fontSize) const;

        // Bind atlas texture of font
        virtual void bindAtlasTexture(FontSize fontSize, uint slot = 0, bool linearFiltering = false) const;

    private:

        // Members
        GLuint mTexture;
    };
}

#endif // EMPTY_FONT_H_
