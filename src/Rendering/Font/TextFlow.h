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

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "Font.h"
#include "Rendering/Shader.h"

#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    class TextFlow
    {
    public:

        // Constructor
        TextFlow(
            Font const * pFont,
            FontSize fontSize,
            Shader const * pShader,
            int width,
            int height,
            std::u16string content);

        // Destructor
        virtual ~TextFlow();

        // Update
        void update(
            int width,
            int height,
            std::u16string content);

        // Draw
        void draw(int xPosition, int yPosition) const;

    private:

        // Calculate mesh
        void calculateMesh();

        // Members
        Font const * mpFont;
        FontSize mFontSize;
        Shader const * mpShader;
        int mWidth;
        int mHeight;
        std::u16string mContent;

        GLuint mVertexCount;
        GLuint mVertexBuffer;
        GLuint mTextureCoordinateBuffer;
        GLuint mVertexArrayObject;
        GLuint mAtlasTextureId;

    };
}

#endif // TEXT_FLOW_H_
