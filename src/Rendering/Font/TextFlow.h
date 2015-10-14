//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO: All values pixel values!

// TODO
//  - Alignment (Right, left, justify, center)
//  - Choose tall, medium, small font (enumeration in eyeGUI.h necessary)
//  - Actually, one would have to have one mesh per atlas texture. Todo for later
//  when there could be more then one atlas texture
//  - will get problems if pointer to font is null
//  - position should be changed using the matrix (much faster!)

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "Font.h"
#include "Rendering/Shader.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

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
            Shader const * pShader,
            int x,
            int y,
            int width,
            int height,
            glm::vec4 color,
            std::u16string content);

        // Destructor
        virtual ~TextFlow();

        // Move
        void move(int x, int y);

        // Set text color
        void setColor(glm::vec4 color);

        // Update
        void update(int width,int height);
        void update(std::u16string content);
        void update(
            int width,
            int height,
            std::u16string content);

        // Resize
        void resize();

        // Draw (uses orthoprojection to scale to screen)
        void draw(float alpha) const;

    private:

        // Calculate mesh (in pixel coordinates)
        void calculateMesh();

        // Members
        GUI const * mpGUI;
        Font const * mpFont;
        FontSize mFontSize;
        Shader const * mpShader;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
        glm::vec4 mColor;
        std::u16string mContent;

        GLuint mVertexCount;
        GLuint mVertexBuffer;
        GLuint mTextureCoordinateBuffer;
        GLuint mVertexArrayObject;
        GLuint mAtlasTextureId;

    };
}

#endif // TEXT_FLOW_H_
