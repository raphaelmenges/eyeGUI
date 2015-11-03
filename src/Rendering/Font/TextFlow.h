//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Creates geometry for given font and text content. Is delegated by elements
// for font rendering in general. All values in this class are in pixel space,
// even floats. Internal the origin is at lower left but all methods
// expect values in a coordinate system with an origin at the upper left.

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "Font.h"
#include "Rendering/Shader.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <vector>

namespace eyegui
{
	// Forward declaration
	class GUI;

	class TextFlow
	{
	public:

		// Constructor
		TextFlow(
			GUI const * pGUI,
			Font const * pFont,
			FontSize fontSize,
			TextFlowAlignment alignment,
			TextFlowVerticalAlignment verticalAlignment,
			Shader const * pShader,
			std::u16string content);

		// Destructor
		virtual ~TextFlow();

		// Set content
		void setContent(std::u16string content);

		// Transform and size (has to be called before first usage)
		void transformAndSize(
			int x,
			int y,
			int width,
			int height);

		// Draw (uses orthoprojection to scale to screen)
		void draw(float scale, glm::vec4 color) const;

	private:

		// Some struct for easier alignment
		struct Word
		{
			std::shared_ptr<std::vector<glm::vec3> > spVertices;
			std::shared_ptr<std::vector<glm::vec2> > spTextureCoordinates;
			float pixelWidth;
		};

		// Calculate mesh (in pixel coordinates)
		void calculateMesh();

		// Calculate word
		Word calculateWord(std::u16string content);

		// Members
		GUI const * mpGUI;
		Font const * mpFont;
		FontSize mFontSize;
		TextFlowAlignment mAlignment;
		TextFlowVerticalAlignment mVerticalAlignment;
		int mX;
		int mY;
		int mWidth;
		int mHeight;
		glm::vec4 mColor;
		std::u16string mContent;
		int mFlowHeight;

		Shader const * mpShader;
		GLuint mVertexCount;
		GLuint mVertexBuffer;
		GLuint mTextureCoordinateBuffer;
		GLuint mVertexArrayObject;
		GLuint mAtlasTextureHandle;
	};
}

#endif // TEXT_FLOW_H_
