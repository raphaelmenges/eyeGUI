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
        TextFlowAlignment alignment,
        Shader const * pShader,
        int x,
        int y,
        int width,
        int height,
        glm::vec4 color,
        std::u16string content)
    {
        // Fill members
        mpGUI = pGUI;
        mpFont = pFont;
        mFontSize = fontSize;
        mAlignment = alignment;
        mpShader = pShader;
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
        mColor = color;
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

    void TextFlow::move(int x, int y)
    {
        mX = x;
        mY = y;
    }

    void TextFlow::setColor(glm::vec4 color)
    {
        mColor = color;
    }

    void TextFlow::update(int width, int height)
    {
        update(width, height, mContent);
    }

    void TextFlow::update(std::u16string content)
    {
        update(mWidth, mHeight, content);
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

    void TextFlow::resize()
    {
        // Calculate mesh
        calculateMesh();
    }

    void TextFlow::draw(float alpha) const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // If matrix were not calculated every frame, this would have to be notified about resizing...
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(mX, mpGUI->getWindowHeight() - mY - mHeight, 0));
        matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix;

        // TODO: TEST (only one atlas at the moment)
        glBindTexture(GL_TEXTURE_2D, mAtlasTextureId);

        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("alpha", alpha);
        mpShader->fillValue("color", mColor);

        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextFlow::calculateMesh()
    {
        // Save currently set buffer
        GLint oldBuffer;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);

        // Get size of space
        int space = mpFont->getGlyph(mFontSize, u' ')->advance.x;

        // Go over words in content
        std::u16string copyContent = mContent;

        // Get words out of string
        std::vector<TextFlow::Word> words;
        std::u16string delimiter = u" ";
        size_t pos = 0;
        std::u16string token;
        while ((pos = copyContent.find(delimiter)) != std::u16string::npos)
        {
            token = copyContent.substr(0, pos);
            words.push_back(calculateWord(token));
            copyContent.erase(0, pos + delimiter.length());
        }
        words.push_back(calculateWord(copyContent)); // Last word

        // Build flow together from words
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoordinates;

        int xPen = 0;
        int yPen = - mpFont->getLineHeight(mFontSize); // First line should be also inside flow

        // Collect line
        /*std::vector<Word const *> line;
        while(xPen + rWord.width > mWidth) */

        // Go over lines
        for(const Word& rWord : words)
        {
            bool addSpace = true;

            // Check, whether word will fit into line
            if(xPen + rWord.width > mWidth)
            {
                xPen = 0;
                yPen -= mpFont->getLineHeight(mFontSize);
                addSpace = false;

                // yPen in another coordinate sysstem than mY and mHeight...
                if((-yPen) > (mY + mHeight))
                {
                    break; // Enough words to fill flow
                }
            }

            // Assuming, that the count of vertices and texture coordinates is equal
            for(int i = 0; i < rWord.spVertices->size(); i++)
            {
                glm::vec3& rVertex = rWord.spVertices->at(i);
                vertices.push_back(glm::vec3(rVertex.x + xPen, rVertex.y + yPen, rVertex.z));
                glm::vec2& rTextureCoordinate = rWord.spTextureCoordinates->at(i);
                textureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
            }
            xPen += rWord.width;

            if(addSpace)
            {
                // No end of line so add space
                xPen += space;
            }
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

    TextFlow::Word TextFlow::calculateWord(std::u16string content)
    {
        // Empty word
        Word word;
        word.spVertices = std::shared_ptr<std::vector<glm::vec3> >(new std::vector<glm::vec3>);
        word.spTextureCoordinates = std::shared_ptr<std::vector<glm::vec2> >(new std::vector<glm::vec2>);

        // Create data for text
        int xPen = 0;
        for(int i = 0; i < content.size(); i++)
        {
            Glyph const * pGlyph = mpFont->getGlyph(mFontSize, content[i]);
            if(pGlyph == NULL)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "TextFlow has character in content not covered by character set");
                continue;
            }

            int yPen = 0 - (pGlyph->size.y - pGlyph->bearing.y);

            // Vertices for this quad
            glm::vec3 vertexA = glm::vec3(xPen, yPen, 0);
            glm::vec3 vertexB = glm::vec3(xPen+pGlyph->size.x, yPen, 0);
            glm::vec3 vertexC = glm::vec3(xPen+pGlyph->size.x, yPen+pGlyph->size.y, 0);
            glm::vec3 vertexD = glm::vec3(xPen, yPen+pGlyph->size.y, 0);

            // Texture coordinates for this quad
            glm::vec2 textureCoordinateA = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateB = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateC = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w);
            glm::vec2 textureCoordinateD = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w);

            xPen += pGlyph->advance.x;

            // Fill into data blocks
            word.spVertices->push_back(vertexA);
            word.spVertices->push_back(vertexB);
            word.spVertices->push_back(vertexC);
            word.spVertices->push_back(vertexC);
            word.spVertices->push_back(vertexD);
            word.spVertices->push_back(vertexA);

            word.spTextureCoordinates->push_back(textureCoordinateA);
            word.spTextureCoordinates->push_back(textureCoordinateB);
            word.spTextureCoordinates->push_back(textureCoordinateC);
            word.spTextureCoordinates->push_back(textureCoordinateC);
            word.spTextureCoordinates->push_back(textureCoordinateD);
            word.spTextureCoordinates->push_back(textureCoordinateA);
        }

        // Set width of whole word
        word.width = xPen;

        return word;
    }
}
