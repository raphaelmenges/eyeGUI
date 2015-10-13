//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

#include "GUI.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

// TODO: Testing
#include <iostream>

namespace eyegui
{
    TextFlow::TextFlow(
        GUI const * pGUI,
        Font const * pFont,
        FontSize fontSize,
        Shader const * pShader,
        int x,
        int y,
        int width,
        int height,
        std::u16string content)
    {
        // Fill members
        mpGUI = pGUI;
        mpFont = pFont;
        mFontSize = fontSize;
        mpShader = pShader;
        mX = x;
        mY = y;
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

        // TODO: just useable when only one atlas texture (whole member will be deleted)
        mAtlasTextureId = mpFont->getGlyph(mFontSize, u'a')->atlasTextureId; // TODO

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

        // Calculate mesh
        calculateMesh();
    }

    void TextFlow::draw(float alpha) const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // If matrix were not calculated every frame, this would have to be notified about resizing...
        glm::mat4 matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1));

        // TODO
        // Position
        // Color?

        // TODO: TEST
        glBindTexture(GL_TEXTURE_2D, mAtlasTextureId);

        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("alpha", alpha);

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
            0.0,0.0,0, 100.0,0,0, 100.0,100.0,0,
            100.0,100.0,0, 0,100.0,0, 0,0,0
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
