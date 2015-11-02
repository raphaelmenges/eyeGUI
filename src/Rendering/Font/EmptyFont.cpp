//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "EmptyFont.h"

#include <vector>

namespace eyegui
{
	EmptyFont::EmptyFont() : Font()
	{
		// Create some empty texture just in case...
		glGenTextures(1, &mTexture);

		std::vector<GLubyte> emptyData(4 * 4, 0);
		glBindTexture(GL_TEXTURE_2D, mTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			4,
			4,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			emptyData.data());

		glBindTexture(GL_TEXTURE_2D, 0);
	}

	EmptyFont::~EmptyFont()
	{
		glDeleteTextures(1, &mTexture);
	}

	void EmptyFont::resizeFontAtlases(int windowHeight)
	{
		// Nothing to do
	}

	Glyph const * EmptyFont::getGlyph(FontSize fontSize, char16_t character) const
	{
		return NULL;
	}

	float EmptyFont::getLineHeight(FontSize fontSize) const
	{
		return 0;
	}

	uint EmptyFont::getAtlasTextureHandle(FontSize fontSize) const
	{
		return mTexture;
	}

}
