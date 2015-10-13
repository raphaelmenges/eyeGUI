//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

namespace eyegui
{
    TextFlow::TextFlow(
        Font const * pFont,
        FontSize fontSize,
        Shader const * pShader,
        int width,
        int height,
        std::u16string content)
    {
        // Fill members
        mpFont = pFont;
        mFontSize = fontSize;
        mpShader = pShader;
        mWidth = width;
        mHeight = height;
        mContent = content;

        // Save currently set buffer and vertex array object
        GLint oldBuffer, oldVAO;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

        // Initialize mesh buffers and vertex array object
        glGenBuffers(1, &mVertexBuffer);
        glGenBuffers(1, &mTextureCoordinateBuffer);
        glGenVertexArrays(1, &mVertexArrayObject);

        // Bind stuff to vertex array object
        glBindVertexArray(mVertexArrayObject);

        // Vertices
        GLuint vertexAttrib = glGetAttribLocation(mpShader->getShaderProgram(), "posAttribute");
        glEnableVertexAttribArray(vertexAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Texture coordinates
        GLuint uvAttrib = glGetAttribLocation(mpShader->getShaderProgram(), "uvAttribute");
        glEnableVertexAttribArray(uvAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Restore old settings
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
        glBindVertexArray(oldVAO);

        // Calculate it
        calculateMesh();
    }

    TextFlow::~TextFlow()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    void TextFlow::update(
        int width,
        int height,
        std::u16string content)
    {
        // Save values
        mWidth = width;
        mHeight = height;
        mContent = content;

        // TODO: just useable when only one atlas texture
        mAtlasTextureId = 42; // TODO

        // Calculate mesh
        calculateMesh();
    }

    void TextFlow::draw(int xPosition, int yPosition) const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // TODO
        // Position
        // Color?

        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextFlow::calculateMesh()
    {
        // Save currently set buffer
        GLint oldBuffer;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);

        // TODO (create real geometry for letters...

        std::vector<float> vertices =
        {
            0.0,0.0,0, 1.0,0,0, 1.0,1.0,0,
            1.0,1.0,0, 0,1.0,0, 0,0,0
        };
        std::vector<float> textureCoordinates =
        {
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0
        };
        mVertexCount = 6;

        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);

        // Restore old setting
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }
}
