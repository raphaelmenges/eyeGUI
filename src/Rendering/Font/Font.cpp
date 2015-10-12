#include "Font.h"

#include "OperationNotifier.h"

namespace eyegui
{
	Font::Font(std::string filepath, std::unique_ptr<FT_Face> upFace, const std::set<wchar_t>& characterSet, int windowWidth, int windowHeight)
	{
		// Fill members
		mFilepath = filepath;
		mupFace = std::move(upFace);
		mCharacterSet = characterSet;

		// Initialize pixel heights (TODO)
		mTallPixelHeight = 100;
		mMediumPixelHeight = 50;
		mSmallPixelHeight = 10;

		// Initilialize textures
		glGenTextures(1, &mTallTexture);
		glGenTextures(1, &mMediumTexture);
		glGenTextures(1, &mSmallTexture);

		// TODO: somewhere: get max texture size for this gpu

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

	void Font::fillAtlas(int pixelHeight, std::map<wchar_t, Glyph>& rGlyphMap, GLuint textureId)
	{
		// Store bitmaps temporarily
		std::vector<FT_Bitmap> bitmaps;

		// Set the height for generation of glyphs
		FT_Set_Pixel_Sizes(*(mupFace.get()), 0, pixelHeight);

		// Go over character set and collect glyphs and bitmaps
		for (wchar_t c : mCharacterSet)
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

			// Fetch current glyph and store it in vector
			bitmaps.push_back((*(mupFace.get()))->glyph->bitmap);

			// Save some values of the glyph
			rGlyphMap[c].advance = (*(mupFace.get()))->glyph->advance.x;
		}

		// TODO: Padding size in percent should be better and defined somewhere else
		int padding = 2;

		// Decide, which resolution is minimum
		int sumPixelWidth = 0;
		for (const FT_Bitmap& rBitmap : bitmaps)
		{
			sumPixelWidth += rBitmap.width + 2 * padding;
		}
		int pixelCount = (pixelHeight + 2 * padding) * sumPixelWidth;
		float minimalPixelRes = sqrt((float)pixelCount);
		
		int i = 8;
		while (pow(2, i) < minimalPixelRes)
		{
			i++;
		}
		int minimumPixelRes = pow(2, i);

		// Try to fit it with given resolution
		int column = 0;
		int row = 0;
		for (const FT_Bitmap& rBitmap : bitmaps)
		{
			sumPixelWidth += rBitmap.width + 2 * padding;
		}


		// Write bitmaps into texture
		// Padding! (must be kept in mind when filling atlas position and size)

		//rGlyphMap[c].size = glm::ivec2()
		// bearing

		/* glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RED,
		face->glyph->bitmap.width,
		face->glyph->bitmap.rows,
		0,
		GL_RED,
		GL_UNSIGNED_BYTE,
		face->glyph->bitmap.buffer);

		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		*/
	}
}

/*


// Now store character for later use
Character character = {
texture,
glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
(GLuint) face->glyph->advance.x
};
characters.insert(std::pair<GLchar, Character>(c, character));
}
}
 */
