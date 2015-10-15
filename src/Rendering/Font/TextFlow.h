//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO:
//  - All values pixel values!
//  - internal origin lower left but interface upper left...
//      - element seems although to handle it at drawing only...
//      - x y coordinate of flow is upper left corner. Flow has to be adjusted,
//      atm it is starting at the bottom...
//      - just say in the description: internally, that coordinate system,
//      and for the interface that.
//  - Actually, one would have to have one mesh per atlas texture. Todo for later
//  when there could be more then one atlas texture
//  - Future work: add "..." when no space left, make font smaller when not enough
//  space available...

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
            TextFlowAlignment alignment,
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

        // Some struct for easier alignment
        struct Word
        {
            std::shared_ptr<std::vector<glm::vec3> > spVertices;
            std::shared_ptr<std::vector<glm::vec2> > spTextureCoordinates;
            float width;
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
