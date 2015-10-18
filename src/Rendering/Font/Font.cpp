//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Font.h"

#include "OperationNotifier.h"
#include "Defines.h"
#include <algorithm>

// TESTING TODO
#include <iostream>

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

        // Update pixel heights
        fillPixelHeights(windowHeight);

        // Fill atlases the first time
        fillAtlases();
    }

    Font::~Font()
    {
        // Delete textures
		for (GLuint handle : mTallTextures)
		{
			glDeleteTextures(1, &handle);
		}
		for (GLuint handle : mMediumTextures)
		{
			glDeleteTextures(1, &handle);
		}
		for (GLuint handle : mSmallTextures)
		{
			glDeleteTextures(1, &handle);
		}

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

    float Font::getLineHeight(FontSize fontSize) const
    {
        // Values seems to be not correct (not depending on bitmap size)
        /*switch(fontSize)
        {
        case FontSize::TALL:
            return mTallLinePixelHeight;
            break;
        case FontSize::MEDIUM:
            return mMediumLinePixelHeight;
            break;
        case FontSize::SMALL:
            return mSmallLinePixelHeight;
            break;
        }*/

        switch(fontSize)
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
		default:
			return (float)mMediumPixelHeight;
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
            mTallLinePixelHeight,
            mTallTextures,
            calculatePadding(mTallPixelHeight));
        fillAtlas(
            mMediumPixelHeight,
            mMediumGlyphs,
            mMediumLinePixelHeight,
            mMediumTextures,
            calculatePadding(mMediumPixelHeight));
        fillAtlas(
            mSmallPixelHeight,
            mSmallGlyphs,
            mSmallLinePixelHeight,
            mSmallTextures,
            calculatePadding(mSmallPixelHeight));
    }

    void Font::fillAtlas(
        int pixelHeight,
        std::map<char16_t, Glyph>& rGlyphMap,
        float& rLineHeight,
		std::vector<GLuint>& rTextures,
        int padding)
    {
		// Get the maximum resolution of textures to decide how big the atlases may be
		int maxTextureResolution;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureResolution);

		// TODO: testing
		maxTextureResolution = 256;

        // Some structure, to keep track of combination of each glyph and its bitmap
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
                    "Failed to find following character in font file: " + c,
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

		// Limit resolution with max texture resolution available
        int xResolution = std::min(maxTextureResolution, (int)pow(2, i));

		// TODO: if one glyph in pixels wider than xResolution...yeah, not good

        // Try to fit it with given resolution
        std::vector<int> rows;

		// Vector of vector saving glyphBitmapPairs
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

		// Determine, how many rows fit into given maximum resolution
		int fittingRows = maxTextureResolution / (pixelHeight + 2 * padding);

		// Determine, how many textures are necessary
		int textureCount = (int)std::ceil((float)rows.size() / fittingRows);
			
		// Add textures if necessary
		if (rTextures.size() < textureCount)
		{
			int necessaryTextures = (textureCount - (int)rTextures.size());

			// Add new textures and generate them
			for (int i = 0; i < necessaryTextures; i++)
			{
				GLuint textureHandle = 0;
				glGenTextures(1, &textureHandle);

				glBindTexture(GL_TEXTURE_2D, textureHandle);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
				glBindTexture(GL_TEXTURE_2D, 0);

				rTextures.push_back(textureHandle);
			}
		}

		// y resolution of all texture atlases but the last
		int yResolution = maxTextureResolution;

		// y resolution of last texture atlas (maybe does not need whole resolution)
		int lastYResolution = ((int)rows.size() - fittingRows * (textureCount - 1)) / (pixelHeight + 2 * padding);

        // Calculate necessary y resolution to have something with power of two
        i = 0;
        while (pow(2, i) < lastYResolution)
        {
            i++;
        }
		lastYResolution = (int)pow(2, i);

		// Clean all atlases
		for (int i = 0; i < textureCount; i++)
		{
			int currentYResolution = yResolution;
			if (i == textureCount - 1)
			{
				currentYResolution = lastYResolution;
			}

			glBindTexture(GL_TEXTURE_2D, rTextures[i]);

			std::vector<GLubyte> emptyData(xResolution * currentYResolution, 0);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				GL_RED,
				xResolution,
				currentYResolution,
				0,
				GL_RED,
				GL_UNSIGNED_BYTE,
				emptyData.data());
		}

        // Enable writing of non power of two
        GLint oldUnpackAlignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Write bitmaps into texture and save further values to the glyph
		int boundTextureAtlas = 0;
		glBindTexture(GL_TEXTURE_2D, rTextures[boundTextureAtlas]);

        int yPen = - pixelHeight - 2 * padding;
		int localRow = 0;
        for(int i = 0; i < bitmapOrder.size(); i++)
        {
			// Check, whether next texture atlas should be used
			if (localRow > fittingRows)
			{
				boundTextureAtlas++;
				glBindTexture(GL_TEXTURE_2D, rTextures[boundTextureAtlas]);
				yPen = -pixelHeight - 2 * padding;
				localRow = 0;
			}

			// Go on and draw one row of glyphs on texture
            int xPen = 0;
            for(int j = 0; j < bitmapOrder[i].size(); j++)
            {
                int bitmapWidth = bitmapOrder[i][j]->first->size.x;
                int bitmapHeight = bitmapOrder[i][j]->first->size.y;

				// Move y pen to top
				int localYPen = yPen + yResolution;

                // Write into texture
                glTexSubImage2D(
                    GL_TEXTURE_2D,
                    0,
                    xPen + padding,
					localYPen + padding,
                    bitmapWidth,
                    bitmapHeight,
                    GL_RED,
                    GL_UNSIGNED_BYTE,
                    bitmapOrder[i][j]->second.data());

                // Save further values to glyph structur
				bitmapOrder[i][j]->first->atlasTextureHandle = boundTextureAtlas;
                bitmapOrder[i][j]->first->atlasPosition = glm::vec4(
                    (float)(xPen + padding) / xResolution,
                    (float)(localYPen + padding) / yResolution,
                    (float)(xPen + padding + bitmapWidth) / xResolution,
                    (float)(localYPen + padding + bitmapHeight) / yResolution);

                // Advance pen
                xPen += bitmapWidth + 2 * padding;
            }

			// Next local row
			localRow++;

            // Advance pen
            yPen -= pixelHeight + 2 * padding;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Reset unpack alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpackAlignment);
    }
}
