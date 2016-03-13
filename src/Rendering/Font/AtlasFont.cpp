//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "AtlasFont.h"

#include "GUI.h"
#include "src/Utilities/OperationNotifier.h"
#include "Defines.h"
#include <algorithm>

namespace eyegui
{
    AtlasFont::AtlasFont(
        GUI const * pGUI,
        std::string filepath,
        std::unique_ptr<FT_Face> upFace,
        std::set<char16_t> characterSet) : Font()
    {
        // Fill members
        mpGUI = pGUI;
        mFilepath = filepath;
        mupFace = std::move(upFace);
        mCharacterSet = characterSet;

        // Initilialize textures
        glGenTextures(1, &mTallTexture);
        glGenTextures(1, &mMediumTexture);
        glGenTextures(1, &mSmallTexture);
        glGenTextures(1, &mKeyboardTexture);

        // Update pixel heights
        fillPixelHeights();

        // Fill atlases the first time
        fillAtlases();
    }

    AtlasFont::~AtlasFont()
    {
        // Delete textures
        glDeleteTextures(1, &mTallTexture);
        glDeleteTextures(1, &mMediumTexture);
        glDeleteTextures(1, &mSmallTexture);
        glDeleteTextures(1, &mKeyboardTexture);

        // Delete used face
        FT_Done_Face(*(mupFace.get()));
    }

    void AtlasFont::resizeFontAtlases()
    {
        // Update pixel heights
        fillPixelHeights();

        // Update all atlases
        fillAtlases();
    }

    Glyph const * AtlasFont::getGlyph(FontSize fontSize, char16_t character) const
    {
        Glyph const * pGlyph = NULL;

        switch (fontSize)
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
        case FontSize::KEYBOARD:
            pGlyph = getGlyph(mKeyboardGlyphs, character);
            break;
        }

        // Check whether key was found
        if (pGlyph == NULL)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Failed to find a character, check font file and character set",
                mFilepath);

            // Try to load fallback
            switch (fontSize)
            {
            case FontSize::TALL:
                pGlyph = getGlyph(mTallGlyphs, FONT_FALLBACK_CHARACTER);
                break;
            case FontSize::MEDIUM:
                pGlyph = getGlyph(mMediumGlyphs, FONT_FALLBACK_CHARACTER);
                break;
            case FontSize::SMALL:
                pGlyph = getGlyph(mSmallGlyphs, FONT_FALLBACK_CHARACTER);
                break;
            case FontSize::KEYBOARD:
                pGlyph = getGlyph(mKeyboardGlyphs, FONT_FALLBACK_CHARACTER);
                break;
            }

            // Check fallback
            if (pGlyph == NULL)
            {
                throwError(
                    OperationNotifier::Operation::RUNTIME,
                    "Fallback character not found, check font file and character set",
                    mFilepath);
            }
        }

        return pGlyph;
    }

    float AtlasFont::getLineHeight(FontSize fontSize) const
    {
        // At the moment, same as target glyph height
        return getTargetGlyphHeight(fontSize);
    }

    float AtlasFont::getTargetGlyphHeight(FontSize fontSize) const
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

        switch (fontSize)
        {
        case FontSize::TALL:
            return (float)mTallPixelHeight;
            break;
        case FontSize::MEDIUM:
            return (float)mMediumPixelHeight;
            break;
        case FontSize::SMALL:
            return (float)mSmallPixelHeight;
            break;
        case FontSize::KEYBOARD:
            return (float)mKeyboardPixelHeight;
            break;
        default:
            return 0;
        }
    }

    void AtlasFont::bindAtlasTexture(FontSize fontSize, uint slot, bool linearFiltering) const
    {
        // Choose slot
        glActiveTexture(GL_TEXTURE0 + slot);

        // Bind atlas texture
        switch (fontSize)
        {
        case FontSize::TALL:
            glBindTexture(GL_TEXTURE_2D, mTallTexture);
            break;
        case FontSize::MEDIUM:
            glBindTexture(GL_TEXTURE_2D, mMediumTexture);
            break;
        case FontSize::SMALL:
            glBindTexture(GL_TEXTURE_2D, mSmallTexture);
            break;
        case FontSize::KEYBOARD:
            glBindTexture(GL_TEXTURE_2D, mKeyboardTexture);
            break;
        }

        // Set sampling
        if (linearFiltering)
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        }
        else
        {
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        }
    }

    Glyph const * AtlasFont::getGlyph(const std::map<char16_t, Glyph>& rGlyphMap, char16_t character) const
    {
        auto it = rGlyphMap.find(character);

        if (it != rGlyphMap.end())
        {
            return &(it->second);
        }
        else
        {
            return NULL;
        }
    }

    int AtlasFont::calculatePadding(int pixelHeight)
    {
        return std::max(
            FONT_MINIMAL_CHARACTER_PADDING,
            (int)(pixelHeight * FONT_CHARACTER_PADDING));
    }

    void AtlasFont::fillPixelHeights()
    {
        float windowHeight =(float) mpGUI->getWindowHeight();
        mTallPixelHeight = (int)(windowHeight * mpGUI->getSizeOfFont(FontSize::TALL));
        mMediumPixelHeight = (int)(windowHeight * mpGUI->getSizeOfFont(FontSize::MEDIUM));
        mSmallPixelHeight = (int)(windowHeight * mpGUI->getSizeOfFont(FontSize::SMALL));
        mKeyboardPixelHeight = (int)(windowHeight * mpGUI->getSizeOfFont(FontSize::KEYBOARD));
    }

    void AtlasFont::fillAtlases()
    {
        fillAtlas(
            mTallPixelHeight,
            mTallGlyphs,
            mTallLinePixelHeight,
            mTallTexture,
            calculatePadding(mTallPixelHeight));
        fillAtlas(
            mMediumPixelHeight,
            mMediumGlyphs,
            mMediumLinePixelHeight,
            mMediumTexture,
            calculatePadding(mMediumPixelHeight));
        fillAtlas(
            mSmallPixelHeight,
            mSmallGlyphs,
            mSmallLinePixelHeight,
            mSmallTexture,
            calculatePadding(mSmallPixelHeight));
        fillAtlas(
            mKeyboardPixelHeight,
            mKeyboardGlyphs,
            mKeyboardLinePixelHeight,
            mKeyboardTexture,
            calculatePadding(mKeyboardPixelHeight));
    }

    void AtlasFont::fillAtlas(
        int pixelHeight,
        std::map<char16_t, Glyph>& rGlyphMap,
        float& rLineHeight,
        GLuint textureHandle,
        int padding)
    {
        // Some typedef, keeps track of combination of each glyph and its bitmap
        typedef std::pair<Glyph*, std::vector<unsigned char> > glyphBitmapPair;

        // Store bitmaps temporarily
        std::vector<glyphBitmapPair> bitmaps;

        // Reference to face
        FT_Face& rFace = *(mupFace.get());

        // Set the height for generation of glyphs
        FT_Set_Pixel_Sizes(rFace, 0, pixelHeight);

        // Set line height
        rLineHeight = (float)(rFace->height) / 64; // Given in 1/64 pixel

        // Go over character set and collect glyphs and bitmaps
        for (char16_t c : mCharacterSet)
        {
            // Load current glyph in face
            if (FT_Load_Char(rFace, c, FT_LOAD_RENDER))
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "Failed to find character in font file, check coverage of font",
                    mFilepath);
                continue;
            }

            // Determine width and height
            int bitmapWidth = rFace->glyph->bitmap.width;
            int bitmapHeight = rFace->glyph->bitmap.rows;

            // Save some values of the glyph
            rGlyphMap[c].advance = glm::vec2(
                (float)(rFace->glyph->advance.x) / 64,   // Given in 1/64 pixel
                (float)(rFace->glyph->advance.y) / 64);  // Given in 1/64 pixel
            rGlyphMap[c].size = glm::ivec2(bitmapWidth, bitmapHeight);
            rGlyphMap[c].bearing = glm::ivec2(
                rFace->glyph->bitmap_left,
                rFace->glyph->bitmap_top);

            // Go over rows and write it mirrored into own buffer
            std::vector<unsigned char> mirrorBuffer;
            for (int i = bitmapHeight - 1; i >= 0; i--)
            {
                for (int j = 0; j < bitmapWidth; j++)
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

        // Get the maximum resolution of textures on this GPU
        int maxTextureResolution;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureResolution);

        if (xResolution > maxTextureResolution)
        {
            throwError(
                OperationNotifier::Operation::RUNTIME,
                "Too many and too big glyphs for texture atlas. GPU supported texture size is insufficient",
                mFilepath);
        }

        // Build up rows with vectors of words
        std::vector<int> rows;
        std::vector<std::vector<glyphBitmapPair const *> > bitmapOrder;
        for (const glyphBitmapPair& rGlyphBitmapPair : bitmaps)
        {
            // Try different rows
            int width = rGlyphBitmapPair.first->size.x + 2 * padding;
            bool check = false;

            for (uint j = 0; j < rows.size(); j++)
            {
                // Does this word fit in this row?
                if ((rows[j] + width) < xResolution)
                {
                    rows[j] += width;
                    bitmapOrder[j].push_back(&rGlyphBitmapPair);
                    check = true;
                    break;
                }
            }
            if (!check)
            {
                // No more room in existing rows, add more
                rows.push_back(width);
                std::vector<glyphBitmapPair const *> rowBitmapOrder;
                rowBitmapOrder.push_back(&rGlyphBitmapPair);
                bitmapOrder.push_back(rowBitmapOrder);
            }
        }

        // Calculate necessary rows
        i = 5;
        int yResolution = (int)rows.size() * (pixelHeight + 2 * padding);
        while (pow(2, i) < yResolution)
        {
            i++;
        }

        // Let's make y resolution also power of two
        yResolution = (int)pow(2, i);

        // Do check for vertical resolution as well
        if (yResolution > maxTextureResolution)
        {
            throwError(
                OperationNotifier::Operation::RUNTIME,
                "Too many and too big glyphs for texture atlas. GPU supported texture size is insufficient",
                mFilepath);
        }

        // Enable writing of non power of two
        GLint oldUnpackAlignment = 4;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Initialize texture for atlas (filtering set at texture binding)
        glBindTexture(GL_TEXTURE_2D, textureHandle);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        std::vector<GLubyte> emptyData(xResolution * yResolution, 0);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_R8,
            xResolution,
            yResolution,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            emptyData.data());

        // Write bitmaps into texture and save further values to the glyph
        int yPen = yResolution - pixelHeight - 2 * padding;
        for (uint i = 0; i < bitmapOrder.size(); i++)
        {
            int xPen = 0;
            for (uint j = 0; j < bitmapOrder[i].size(); j++)
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

                // Save further values to glyph structure
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
