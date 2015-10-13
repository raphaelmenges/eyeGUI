//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

#include "GUI.h"
#include "OperationNotifier.h"
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

        // Empty data
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoordinates;

        // Create data for text (TODO: expand it..)
        int xPen = 0;
        for(int i = 0; i < mContent.size(); i++)
        {
            Glyph const * pGlyph = mpFont->getGlyph(mFontSize, mContent[i]);
            if(pGlyph == NULL)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "TextFlow has character in content not covered by character set");
                continue;
            }

            // Vertices for this quad
            glm::vec3 vertexA = glm::vec3(xPen, 0, 0);
            glm::vec3 vertexB = glm::vec3(xPen+pGlyph->size.x, 0, 0);
            glm::vec3 vertexC = glm::vec3(xPen+pGlyph->size.x, pGlyph->size.y, 0);
            glm::vec3 vertexD = glm::vec3(xPen, pGlyph->size.y, 0);

            // Texture coordinates for this quad
            glm::vec2 textureCoordinateA = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateB = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateC = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w);
            glm::vec2 textureCoordinateD = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w);

            //xPen += pGlyph->size.x + pGlyph->advance.x; TODO: advance seems too big
            xPen += pGlyph->size.x;

            // Fill into data blocks
            vertices.push_back(vertexA);
            vertices.push_back(vertexB);
            vertices.push_back(vertexC);
            vertices.push_back(vertexC);
            vertices.push_back(vertexD);
            vertices.push_back(vertexA);

            textureCoordinates.push_back(textureCoordinateA);
            textureCoordinates.push_back(textureCoordinateB);
            textureCoordinates.push_back(textureCoordinateC);
            textureCoordinates.push_back(textureCoordinateC);
            textureCoordinates.push_back(textureCoordinateD);
            textureCoordinates.push_back(textureCoordinateA);
        }

        // Vertex count
        mVertexCount = vertices.size();

        // Fill into buffer
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 3 * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 2 * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);

        // Restore old setting
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }
}
