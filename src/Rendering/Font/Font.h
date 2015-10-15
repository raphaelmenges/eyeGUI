//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

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
        GLint       atlasTextureId; // Id of atlas texture
        glm::vec4	atlasPosition;	// Position in atlas (minU, minV, maxU, maxV)
        glm::ivec2  size;			// Size in pixel
        glm::ivec2	bearing;		// Offset from baseline to left / top of glyph in pixel
        glm::ivec2	advance;        // Offset to advance to next glyph in pixel
    };

    class Font
    {
    public:

        // Constructor (takes responsibility for face)
        Font(
            std::string filepath,
            std::unique_ptr<FT_Face> upFace,
            std::set<char16_t> characterSet,
            int windowHeight);

        // Destructor
        virtual ~Font();

        // Resize font atlases
        void resizeFontAtlases(int windowHeight);

        // Get glyph (may return NULL if not found)
        Glyph const * getGlyph(FontSize fontSize, char16_t character) const;

        // Get height of line
        int getLineHeight(FontSize fontSize) const;

    private:

        // Get glyph (may return NULL if not found)
        Glyph const * getGlyph(const std::map<char16_t, Glyph>& rGlyphMap, char16_t character) const;

        // Calculate padding
        int calculatePadding(int pixelHeight);

        // Fill pixel heights
        void fillPixelHeights(int windowHeight);

        // Fill all atlases
        void fillAtlases();

        // Fill atlas
        void fillAtlas(
            int pixelHeight,
            std::map<char16_t,Glyph>& rGlyphMap,
            int& rLineHeight,
            GLuint textureId,
            int padding);

        // Members
        std::unique_ptr<FT_Face> mupFace;
        std::set<char16_t> mCharacterSet;

        std::map<char16_t, Glyph> mTallGlyphs;
        std::map<char16_t, Glyph> mMediumGlyphs;
        std::map<char16_t, Glyph> mSmallGlyphs;

        int mTallLineHeight;
        int mMediumLineHeight;
        int mSmallLineHeight;

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
