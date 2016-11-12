//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract superclass for text assets.

#ifndef TEXT_H_
#define TEXT_H_

#include "src/Rendering/Font/Font.h"
#include "src/Rendering/Shader.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <vector>

namespace eyegui
{
    // Forward declaration
    class GUI;
    class AssetManager;
	class RenderItem;

    // Typedef
    typedef std::vector<std::pair<glm::vec3, glm::vec2> > RenderWordVertices;

    // Minimal class for renderable word
    class RenderWord
    {
    public:

        // Get letter count (is equal size of x offsets minus one)
        uint getLetterCount() const { return (uint)glm::max(0, (int)xOffsets.size() - 1); }

        // Position and texture coordinate
        std::shared_ptr<RenderWordVertices> spVertices;

        // Width of rendered word
        float pixelWidth;

        // Position after single letters, first one is always zero to indicate position in front of first letter.
		// So there are be letterCount + 1 many offsets
        std::vector<int> xOffsets;
    };

    // Class
    class Text
    {
    public:

        // Constructor
        Text(
            GUI const * pGUI,
            AssetManager* pAssetManager,
            Font const * pFont,
            FontSize fontSize,
            float scale,
            std::u16string content);

		// Copy constructor
		Text(const Text& rOtherText);

        // Destructor
        virtual ~Text() = 0;

        // Draw
        virtual void draw(
			glm::vec4 color,
			float alpha,
			bool renderBackground = false,
			int xOffset = 0,
			int yOffset = 0) const = 0;

        // Set content and call "calculateMesh"
        void setContent(std::u16string content);

		// Get height of line
        float getLineHeight() const ;

    protected:

        // Draws simple background
        void drawSimpleBackground(
            uint width,
            uint height,
            uint textWidth,
            uint textHeight,
            int yAlignmentOffset,
            int xOffset,
            int yOffset,
            float alpha) const;

        // Calculate mesh (in pixel coordinates). Calls specialized method of subclasses
        void calculateMesh();

        // Special calculate mesh implemented by specialized classes
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight,
            RenderWordVertices& rVertices) = 0;

        // Calculate single word and returns it
        RenderWord calculateWord(const std::u16string& rContent, float scale) const;
        RenderWord calculateWord(const char16_t& rLetter, float scale) const;

		// Prepare text
		void prepareText();

        // Members
        GUI const * mpGUI;
        AssetManager* mpAssetManager;
        Font const * mpFont;
        FontSize mFontSize;
        float mScale;
        int mX;
        int mY;
        std::u16string mContent;
		RenderItem const * mpBackground;
        Shader const * mpShader;
        GLuint mVertexCount;
        GLuint mVertexBuffer;
        GLuint mTextureCoordinateBuffer;
        GLuint mVertexArrayObject;
    };
}

#endif // TEXT_H_
