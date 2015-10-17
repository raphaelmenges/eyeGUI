//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Font.h"

#include "OperationNotifier.h"
#include "Defines.h"
#include <algorithm>

namespace eyegui
{
    Font::Font(
        std::string filepath,
        std::unique_ptr<FT_Face> upFace,
        std::set<char16_t> characterSet,
        int windowHeight)
    {
        // Fill members
        mFilepath = filepath;
        mupFace = std::move(upFace);
        mCharacterSet = characterSet;

        // Initialize pixel heights (TODO)
        mTallPixelHeight = 100;
        mMediumPixelHeight = 32;
        mSmallPixelHeight = 10;

        // Initilialize textures
        glGenTextures(1, &mTallTexture);
        glGenTextures(1, &mMediumTexture);
        glGenTextures(1, &mSmallTexture);

        // Update pixel heights
        fillPixelHeights(windowHeight);

        // Fill atlases the first time
        fillAtlases();
    }

    Font::~Font()
    {
        // Delete textures
        glDeleteTextures(1, &mTallTexture);
        glDeleteTextures(1, &mMediumTexture);
        glDeleteTextures(1, &mSmallTexture);

        // Delete used face
        FT_Done_Face(*(mupFace.get()));
    }

    void Font::resizeFontAtlases(int windowHeight)
    {
        // Update pixel heights
        fillPixelHeights(windowHeight);

        // Update all atlases
        fillAtlases();
    }

    Glyph const * Font::getGlyph(FontSize fontSize, char16_t character) const
    {
        Glyph const * pGlyph = NULL;

        switch(fontSize)
        {
        case FontSize::TALL:
            pGlyph = getGlyph(mTallGlyphs, character);
            break;
        case FontSize::MEDIUM:
            pGlyph = getGlyph(mMediumGlyphs, character);
            break;
        case FontSize::SMALL:
            pGlyph = getGlyph(mSmallGlyphs, character);
            break;
        }

        return pGlyph;
    }

    int Font::getLineHeight(FontSize fontSize) const
    {
        // Values seems to be not correct (not depending on bitmap size)
        /*switch(fontSize)
        {
        case FontSize::TALL:
            return mTallLineHeight;
            break;
        case FontSize::MEDIUM:
            return mMediumLineHeight;
            break;
        case FontSize::SMALL:
            return mSmallLineHeight;
            break;
        }*/

        switch(fontSize)
        {
        case FontSize::TALL:
            return mTallPixelHeight;
            break;
        case FontSize::MEDIUM:
            return mMediumPixelHeight;
            break;
        case FontSize::SMALL:
            return mSmallPixelHeight;
            break;
		default:
			return mMediumPixelHeight;
			break;
        }
    }

    Glyph const * Font::getGlyph(const std::map<char16_t, Glyph>& rGlyphMap, char16_t character) const
    {
        auto it = rGlyphMap.find(character);

        if(it != rGlyphMap.end())
        {
            return &(it->second);
        }
        else
        {
            return NULL;
        }
    }

    int Font::calculatePadding(int pixelHeight)
    {
        return std::max(
            FONT_MINIMAL_CHARACTER_PADDING,
            (int) (pixelHeight * FONT_CHARACTER_PADDING));
    }

    void Font::fillPixelHeights(int windowHeight)
    {
        mTallPixelHeight = (int)(windowHeight * FONT_TALL_SCREEN_HEIGHT);
        mMediumPixelHeight = (int)(windowHeight * FONT_MEDIUM_SCREEN_HEIGHT);
        mSmallPixelHeight = (int)(windowHeight * FONT_SMALL_SCREEN_HEIGHT);
    }

    void Font::fillAtlases()
    {
        fillAtlas(
            mTallPixelHeight,
            mTallGlyphs,
            mTallLineHeight,
            mTallTexture,
            calculatePadding(mTallPixelHeight));
        fillAtlas(
            mMediumPixelHeight,
            mMediumGlyphs,
            mMediumLineHeight,
            mMediumTexture,
            calculatePadding(mMediumPixelHeight));
        fillAtlas(
            mSmallPixelHeight,
            mSmallGlyphs,
            mSmallLineHeight,
            mSmallTexture,
            calculatePadding(mSmallPixelHeight));
    }

    void Font::fillAtlas(
        int pixelHeight,
        std::map<char16_t, Glyph>& rGlyphMap,
        int& rLineHeight,
        GLuint textureId,
        int padding)
    {
        // Some typedef, keep track of combination of each glyph and its bitmap
        typedef std::pair<Glyph*, std::vector<unsigned char> > glyphBitmapPair;

        // Store bitmaps temporarily
        std::vector<glyphBitmapPair> bitmaps;

        // Reference to face
        FT_Face& rFace = *(mupFace.get());

        // Set the height for generation of glyphs
        FT_Set_Pixel_Sizes(rFace, 0, pixelHeight);

        // Set line height
        rLineHeight = rFace->height / 64; // Given in 1/64 pixel

        // Go over character set and collect glyphs and bitmaps
        for (char16_t c : mCharacterSet)
        {
            // Load current glyph in face
            if (FT_Load_Char(rFace, c, FT_LOAD_RENDER))
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "Failed to find following character in font file: " + c,
                    mFilepath);
                continue;
            }

            // Determine width and height
            int bitmapWidth = rFace->glyph->bitmap.width;
            int bitmapHeight = rFace->glyph->bitmap.rows;

            // Save some values of the glyph
            rGlyphMap[c].atlasTextureId = textureId;
            rGlyphMap[c].advance = glm::ivec2(
                rFace->glyph->advance.x / 64,   // Given in 1/64 pixel
                rFace->glyph->advance.y / 64);  // Given in 1/64 pixel
            rGlyphMap[c].size = glm::ivec2(bitmapWidth, bitmapHeight);
            rGlyphMap[c].bearing = glm::ivec2(
                rFace->glyph->bitmap_left,
                rFace->glyph->bitmap_top);

            // Go over rows and mirror it into new buffer
            std::vector<unsigned char> mirrorBuffer;
            for(int i = bitmapHeight-1; i >= 0; i--)
            {
                for(int j = 0; j < bitmapWidth; j++)
                {
                    mirrorBuffer.push_back(rFace->glyph->bitmap.buffer[i*bitmapWidth + j]);
                }
            }

            // Fetch current glyph and bitmap and store it in vector as pair
            bitmaps.push_back(
                glyphBitmapPair(
                    &rGlyphMap[c],
                    mirrorBuffer));
        }

        // Decide, which resolution is minimum when texture would be quadratic
        int sumPixelWidth = 0;
        for (const glyphBitmapPair& rGlyphBitmapPair : bitmaps)
        {
            sumPixelWidth += rGlyphBitmapPair.first->size.x + 2 * padding;
        }
        int pixelCount = (pixelHeight + 2 * padding) * sumPixelWidth;
        float sqrtPixelCount = sqrt((float)pixelCount);

        int i = 5;
        while (pow(2, i) < sqrtPixelCount)
        {
            i++;
        }
        int xResolution = (int)pow(2, i);

        // Try to fit it with given resolution, at least in columns
        std::vector<int> rows;
        std::vector<std::vector<glyphBitmapPair const *> > bitmapOrder;
        for (const glyphBitmapPair& rGlyphBitmapPair : bitmaps)
        {
            // Try different columns
            int width = rGlyphBitmapPair.first->size.x + 2 * padding;
            bool check = false;

            for(int j = 0; j < rows.size(); j++)
            {
                if((rows[j] + width) < xResolution)
                {
                    rows[j] += width;
                    bitmapOrder[j].push_back(&rGlyphBitmapPair);
                    check = true;
                    break;
                }
            }
            if(!check)
            {
                // No more room in rows, add more
                rows.push_back(width);
                std::vector<glyphBitmapPair const *> rowBitmapOrder;
                rowBitmapOrder.push_back(&rGlyphBitmapPair);
                bitmapOrder.push_back(rowBitmapOrder);
            }
        }

        // Calculate necessary rows
        i = 0;
        int yResolution = (int)rows.size() * (pixelHeight + 2 * padding);
        while (pow(2, i) < yResolution)
        {
            i++;
        }

        // Let's make y resolution also power of two
        yResolution = (int)pow(2, i);

        // Enable writing of non power of two
        GLint oldUnpackAlignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Initialize texture for atlas
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        std::vector<GLubyte> emptyData(xResolution * yResolution, 0);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            xResolution,
            yResolution,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            emptyData.data());

        // Write bitmaps into texture and save further values to the glyph
        int yPen = yResolution - pixelHeight - 2 * padding;
        for(int i = 0; i < bitmapOrder.size(); i++)
        {
            int xPen = 0;
            for(int j = 0; j < bitmapOrder[i].size(); j++)
            {
                int bitmapWidth = bitmapOrder[i][j]->first->size.x;
                int bitmapHeight = bitmapOrder[i][j]->first->size.y;

                // Write into texture
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    xPen + padding,
                    yPen + padding,
                    bitmapWidth,
                    bitmapHeight,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    bitmapOrder[i][j]->second.data());

                // Save further values to glyph structur
                bitmapOrder[i][j]->first->atlasPosition = glm::vec4(
                    (float)(xPen + padding) / xResolution,
                    (float)(yPen + padding) / yResolution,
                    (float)(xPen + padding + bitmapWidth) / xResolution,
                    (float)(yPen + padding + bitmapHeight) / yResolution);

                // Advance pen
                xPen += bitmapWidth + 2 * padding;
            }

            // Advance pen
            yPen -= pixelHeight + 2 * padding;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Reset unpack alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpackAlignment);
    }
}
