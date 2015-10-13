//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

// TODO:
//	- what to do if glyph was not found?
//  - letters up side down
//  - letters should start at top
//  - some letters not found (maybe test font is not good?!)
//  - fillAtlas VERY slow (maybe no improvement possible..)

#ifndef FONT_H_
#define FONT_H_

#include "eyeGUI.h"

#include "externals/FreeType2/include/ft2build.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"
#include "externals/GLM/glm/glm.hpp"

#include <memory>
#include <map>
#include <set>
#include <vector>

#include FT_FREETYPE_H

namespace eyegui
{
    struct Glyph
    {
        glm::vec2	atlasPosition;	// Position in atlas
        glm::vec2	atlasSize;		// Size in atlas
        glm::ivec2  size;			// Size in pixel
        glm::ivec2	bearing;		// Offset from baseline to left / top of glyph in pixel
        GLuint		advance;		// Offset to advance to next glyph in pixel
    };

    class Font
    {
    public:

        // Constructor (takes responsibility for face)
        Font(
            std::string filepath,
            std::unique_ptr<FT_Face> upFace,
            const std::set<char16_t>& characterSet,
            int windowWidth,
            int windowHeight);

        // Destructor
        virtual ~Font();

        // Resize font atlases
        void resizeFontAtlases(int windowWidth, int windowHeight);

        // For testing (TODO)
        GLuint getMediumTextureHandle() {return mMediumTexture;}

    private:

        // Fill atlas
        void fillAtlas(int pixelHeight, std::map<char16_t, Glyph>& rGlyphMap, GLuint textureId);

        // Members
        std::unique_ptr<FT_Face> mupFace;
        std::set<char16_t> mCharacterSet;

        std::map<char16_t, Glyph> mTallGlyphs;
        std::map<char16_t, Glyph> mMediumGlyphs;
        std::map<char16_t, Glyph> mSmallGlyphs;

        GLuint mTallTexture;
        GLuint mMediumTexture;
        GLuint mSmallTexture;

        int mTallPixelHeight;
        int mMediumPixelHeight;
        int mSmallPixelHeight;

        std::string mFilepath;

    };
}

#endif // FONT_H_
