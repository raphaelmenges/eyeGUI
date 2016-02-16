//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Font implementation using FreeType library to render glyphs as bitmaps
// onto a texture atlas.

#ifndef ATLAS_FONT_H_
#define ATLAS_FONT_H_

#include "Font.h"

#include "externals/FreeType2/include/ft2build.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <memory>
#include <map>
#include <set>
#include <vector>

#include FT_FREETYPE_H

namespace eyegui
{
    class AtlasFont : public Font
    {
    public:

        // Constructor (takes responsibility for face)
        AtlasFont(
            std::string filepath,
            std::unique_ptr<FT_Face> upFace,
            std::set<char16_t> characterSet,
            int windowHeight,
            float fontTallSize,
            float fontMediumSize,
            float fontSmallSize);

        // Destructor
        virtual ~AtlasFont();

        // Resize font atlases
        virtual void resizeFontAtlases(int windowHeight);

        // Get glyph (may return NULL if not found)
        virtual Glyph const * getGlyph(FontSize fontSize, char16_t character) const;

        // Get height of line
        virtual float getLineHeight(FontSize fontSize) const;

        // Bind atlas texture of font
        virtual void bindAtlasTexture(FontSize fontSize, uint slot = 0, bool linearFiltering = false) const;

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
            std::map<char16_t, Glyph>& rGlyphMap,
            float& rLineHeight,
            GLuint textureHandle,
            int padding);

        // Members
        std::unique_ptr<FT_Face> mupFace;
        std::set<char16_t> mCharacterSet;

        float mFontTallSize;
        float mFontMediumSize;
        float mFontSmallSize;

        std::map<char16_t, Glyph> mTallGlyphs;
        std::map<char16_t, Glyph> mMediumGlyphs;
        std::map<char16_t, Glyph> mSmallGlyphs;

        float mTallLinePixelHeight;
        float mMediumLinePixelHeight;
        float mSmallLinePixelHeight;

        GLuint mTallTexture;
        GLuint mMediumTexture;
        GLuint mSmallTexture;

        int mTallPixelHeight;
        int mMediumPixelHeight;
        int mSmallPixelHeight;

        std::string mFilepath;
    };
}

#endif // ATLAS_FONT_H_
