#include "Font.h"

#include "OperationNotifier.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
    Font::Font(
        std::string filepath,
        std::unique_ptr<FT_Face> upFace,
        const std::set<char16_t>& characterSet,
        int windowWidth,
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

        // TODO: somewhere: get max texture size for this gpu and limit something somehow...

        // Fill atlases the first time
        fillAtlas(mTallPixelHeight, mTallGlyphs, mTallTexture);
        fillAtlas(mMediumPixelHeight, mMediumGlyphs, mMediumTexture);
        fillAtlas(mSmallPixelHeight, mSmallGlyphs, mSmallTexture);
    }

    Font::~Font()
    {
        // Delete used face
        FT_Done_Face(*(mupFace.get()));
    }

    void Font::resizeFontAtlases(int windowWidth, int windowHeight)
    {
        // Update pixel heights (TODO)
        mTallPixelHeight = 100;
        mMediumPixelHeight = 50;
        mSmallPixelHeight = 10;

        fillAtlas(mTallPixelHeight, mTallGlyphs, mTallTexture);
        fillAtlas(mMediumPixelHeight, mMediumGlyphs, mMediumTexture);
        fillAtlas(mSmallPixelHeight, mSmallGlyphs, mSmallTexture);
    }

    void Font::fillAtlas(int pixelHeight, std::map<char16_t, Glyph>& rGlyphMap, GLuint textureId)
    {
        // Some typedef, keep track of combination of each glyph and its bitmap
        typedef std::pair<Glyph*, std::vector<unsigned char> > glyphBitmapPair;

        // Store bitmaps temporarily
        std::vector<glyphBitmapPair> bitmaps;

        // Set the height for generation of glyphs
        FT_Set_Pixel_Sizes(*(mupFace.get()), 0, pixelHeight);

        // Go over character set and collect glyphs and bitmaps
        for (char16_t c : mCharacterSet)
        {
            // Load current glyph in face
            if (FT_Load_Char(*(mupFace.get()), c, FT_LOAD_RENDER))
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "Failed to find following character in font file: " + c,
                    mFilepath);
                continue;
            }

            // Determine width and height
            int bitmapWidth = (*(mupFace.get()))->glyph->bitmap.width;
            int bitmapHeight = (*(mupFace.get()))->glyph->bitmap.rows;

            // Start of buffer
            unsigned char* bufferStart = (*(mupFace.get()))->glyph->bitmap.buffer;

            // Save some values of the glyph
            rGlyphMap[c].advance = (*(mupFace.get()))->glyph->advance.x;
            rGlyphMap[c].size = glm::ivec2(bitmapWidth, bitmapHeight);
            rGlyphMap[c].bearing = glm::ivec2(
                (*(mupFace.get()))->glyph->bitmap_left,
                (*(mupFace.get()))->glyph->bitmap_top);

            // Fetch current glyph, copy bitmap and store it in vector as pair
            bitmaps.push_back(
                glyphBitmapPair(
                    &rGlyphMap[c],
                    std::vector<unsigned char>(
                        bufferStart,
                        bufferStart + bitmapWidth * bitmapHeight)));
        }

        // TODO: Padding size in percent! should be better and defined somewhere else
        int padding = 2;

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
        int xResolution = pow(2, i);

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
        int yResolution = rows.size() * (pixelHeight + 2 * padding);
        while (pow(2, i) < yResolution)
        {
            i++;
        }

        // Let's make y resolution also power of two
        yResolution = pow(2, i);

        // Enable writing of non power of two
        GLint oldUnpackAlignment;
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &oldUnpackAlignment);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Initialize texture for atlas
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

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
        int yPen = 0;
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

                xPen += bitmapWidth + 2 * padding;

                // Save further values to glyph structure TODO
            }
            yPen += pixelHeight + 2 * padding;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);

        // Reset unpack alignment
        glPixelStorei(GL_UNPACK_ALIGNMENT, oldUnpackAlignment);
    }
}
