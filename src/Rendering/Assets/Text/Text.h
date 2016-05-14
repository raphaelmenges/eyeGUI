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
        virtual void draw(glm::vec4 color, float alpha, bool renderBackground = false) const = 0;

        // Set content and calls "calculateMesh"
        void setContent(std::u16string content);

    protected:

        // Struct for word
        struct Word
        {
            std::shared_ptr<std::vector<glm::vec3> > spVertices;
            std::shared_ptr<std::vector<glm::vec2> > spTextureCoordinates;
            float pixelWidth;
        };

        // Calculate mesh (in pixel coordinates). Calls specialized method of subclasses
        void calculateMesh();

        // Special calculate mesh implemented by specialized classes
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates) = 0;

        // Calculate single word and returns it
        Word calculateWord(std::u16string content, float scale) const;

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
