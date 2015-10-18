//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

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
		virtual void resizeFontAtlases(int windowHeight);

		// Get glyph (may return NULL if not found)
		virtual Glyph const * getGlyph(FontSize fontSize, char16_t character) const;

		// Get height of line
		virtual float getLineHeight(FontSize fontSize) const;

		// Get handle of texture atlas
		virtual uint getAtlasTextureHandle(FontSize fontSize) const;
	
	private:

		// Members
		GLuint mTexture;
	};
}

#endif // EMPTY_FONT_H_