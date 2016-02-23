//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

#include "GUI.h"
#include "AssetManager.h"
#include "OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

#include <cmath>

namespace eyegui
{
    TextFlow::TextFlow(
        GUI const * pGUI,
        AssetManager* pAssetManager,
        Font const * pFont,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        float scale,
        std::u16string content)
    {
        // Fill members
        mpGUI = pGUI;
        mpAssetManager = pAssetManager;
        mpFont = pFont;
        mFontSize = fontSize;
        mAlignment = alignment;
        mVerticalAlignment = verticalAlignment;
        mScale = scale;
        mContent = content;
        mFlowHeight = 0;

        // Fetch shader
        mpShader = mpAssetManager->fetchShader(shaders::Type::TEXT_FLOW);

        // TransformAndSize has to be called before usage
        mX = 0;
        mY = 0;
        mWidth = 0;
        mHeight = 0;
        mVertexCount = 0;

        // Save currently set buffer and vertex array object
        GLint oldBuffer = -1;
        GLint oldVAO = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
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
    }

    TextFlow::~TextFlow()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    // Set content
    void TextFlow::setContent(std::u16string content)
    {
        mContent = content;
        calculateMesh();
    }

    // Transform and size
    void TextFlow::transformAndSize(
        int x,
        int y,
        int width,
        int height)
    {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
        calculateMesh();
    }

    void TextFlow::draw(
           glm::vec4 color,
           float alpha,
           float activity,
           glm::vec4 dimColor,
           float dim,
           glm::vec4 markColor,
           float mark) const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Calculate y offset because of vertical alignment
        int yOffset;
        switch (mVerticalAlignment)
        {
        case TextFlowVerticalAlignment::TOP:
            yOffset = 0;
            break;
        case TextFlowVerticalAlignment::CENTER:
            yOffset = std::max((mHeight - mFlowHeight) / 2, 0);
            break;
        case TextFlowVerticalAlignment::BOTTOM:
            yOffset = std::max((mHeight - mFlowHeight), 0);
            break;
        }

        // Calculate transformation matrix
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(mX, mpGUI->getWindowHeight() - (mY + yOffset), 0)); // Change coordinate system and translate to position
        matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix; // Pixel to world space

        // Bind atlas texture
        if (mScale == 1.0f)
        {
            mpFont->bindAtlasTexture(mFontSize, 0, false);
        }
        else
        {
            mpFont->bindAtlasTexture(mFontSize, 0, true);
        }

        // Fill uniforms
        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("color", color);
        mpShader->fillValue("alpha", alpha);
        mpShader->fillValue("activity", activity);
        mpShader->fillValue("dimColor", dimColor);
        mpShader->fillValue("dim", dim);
        mpShader->fillValue("markColor", markColor);
        mpShader->fillValue("mark", mark);

        // Draw flow
        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextFlow::calculateMesh()
    {
        // Save currently set buffer
        GLint oldBuffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);

        // Get size of space character
        float pixelOfSpace = 0;

        Glyph const * pGlyph = mpFont->getGlyph(mFontSize, u' ');
        if (pGlyph == NULL)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "TextFlow does not find space sign in font");
        }
        else
        {
            pixelOfSpace = mScale * pGlyph->advance.x;
        }

        // Create mark for overflow
        Word overflowMark = calculateWord(TEXT_FLOW_OVERFLOW_MARK, mScale);

        // Get height of line
        float lineHeight = mScale * mpFont->getLineHeight(mFontSize);

        // Go over words in content
        std::u16string copyContent = mContent;
        size_t pos = 0;
        std::u16string token;

        // Get pararaphs separated by \n
        std::vector<std::u16string> paragraphs;
        std::u16string paragraphDelimiter = u"\n";

        while ((pos = copyContent.find(paragraphDelimiter)) != std::u16string::npos)
        {
            token = copyContent.substr(0, pos);
            paragraphs.push_back(token);
            copyContent.erase(0, pos + paragraphDelimiter.length());
        }
        paragraphs.push_back(copyContent); // Last paragraph (paragraphs never empty)

        // Build flow together from words in paragraphs
        std::vector<glm::vec3> vertices;
        std::vector<glm::vec2> textureCoordinates;

        // Do not generate text flow mesh when there is a failure
        bool failure = false;

        // Go over paragraphs (pens are in local pixel coordinate system with origin in lower left corner of element)
        float yPixelPen = -lineHeight; // First line should be also inside flow
        for (std::u16string& rPargraph : paragraphs)
        {
            // Get words out of paragraph
            std::vector<Word> words;
            std::u16string wordDelimiter = u" ";
            while ((pos = rPargraph.find(wordDelimiter)) != std::u16string::npos)
            {
                token = rPargraph.substr(0, pos);
                rPargraph.erase(0, pos + wordDelimiter.length());
                failure |= !insertWord(words, token, mWidth, mScale);
            }

            // Add last token from paragraph as well
            failure |= !insertWord(words, rPargraph, mWidth, mScale);

            // Failure appeared, forget it
            if (!failure)
            {
                // Prepare some values
                int wordIndex = 0;
                bool hasNext = !words.empty();

                // Go over lines to write paragraph
                while (hasNext && abs(yPixelPen) <= mHeight)
                {
                    // Collect words in one line
                    std::vector<Word const *> line;
                    float wordsPixelWidth = 0;
                    float newWordsWithSpacesPixelWidth = 0;

                    // Still words in the paragraph and enough space? Fill into line!
                    while (hasNext && newWordsWithSpacesPixelWidth <= mWidth)
                    {
                        // First word should always fit into width because of previous checks
                        wordsPixelWidth += words[wordIndex].pixelWidth;
                        line.push_back(&words[wordIndex]);
                        wordIndex++;

                        if (wordIndex >= words.size())
                        {
                            // No words in paragraph left
                            hasNext = false;
                        }
                        else
                        {
                            // Calculate next width of line
                            newWordsWithSpacesPixelWidth = std::ceil(
                                (wordsPixelWidth + (float)words[wordIndex].pixelWidth) // Words size (old ones and new one)
                                + (((float)line.size()) - 1.0f) * pixelOfSpace); // Spaces between words
                        }
                    }

                    // If this is last line and after it still words left, replace it by some mark for overflow
                    if (hasNext && abs(yPixelPen - lineHeight) > mHeight && overflowMark.pixelWidth <= mWidth)
                    {
                        line.clear();
                        wordsPixelWidth = overflowMark.pixelWidth;
                        line.push_back(&overflowMark);
                    }

                    // Decide dynamic space for line
                    float dynamicSpace = pixelOfSpace;
                    if (line.size() > 1)
                    {
                        if (mAlignment == TextFlowAlignment::JUSTIFY && hasNext && line.size() > 1) // Do not use dynamic space for last line
                        {
                            // For justify, do something dynamic
                            dynamicSpace = ((float)mWidth - wordsPixelWidth) / ((float)line.size() - 1.0f);
                        }
                        else
                        {
                            // Adjust space to compensate precision errors in other alignments
                            float calculatedDynamicSpace = (float)mWidth - (wordsPixelWidth / (float)(line.size() - 1));
                            dynamicSpace = std::min(dynamicSpace, calculatedDynamicSpace);
                        }
                    }

                    // Now decide xOffset for line
                    float xOffset = 0;
                    if (mAlignment == TextFlowAlignment::RIGHT || mAlignment == TextFlowAlignment::CENTER)
                    {
                        xOffset = (float)mWidth - ((wordsPixelWidth + ((float)line.size() - 1.0f) * dynamicSpace));
                        if (mAlignment == TextFlowAlignment::CENTER)
                        {
                            xOffset = xOffset / 2.0f;
                        }
                    }

                    // Combine word geometry to one line
                    float xPixelPen = xOffset;
                    for (int i = 0; i < line.size(); i++)
                    {
                        // Assuming, that the count of vertices and texture coordinates is equal
                        for (int j = 0; j < line[i]->spVertices->size(); j++)
                        {
                            const glm::vec3& rVertex = line[i]->spVertices->at(j);
                            vertices.push_back(glm::vec3(rVertex.x + xPixelPen, rVertex.y + yPixelPen, rVertex.z));
                            const glm::vec2& rTextureCoordinate = line[i]->spTextureCoordinates->at(j);
                            textureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
                        }

                        // Advance xPen
                        xPixelPen += dynamicSpace + line[i]->pixelWidth;
                    }

                    // Advance yPen
                    yPixelPen -= lineHeight;
                }
            }
        }

        // If failure appeared, clean up
        if (failure)
        {
            // Vertex count will become zero
            vertices.clear();
            textureCoordinates.clear();
        }

        // Get height of all lines (yPixelPen is one line to low now)
        mFlowHeight = (int)std::max(std::ceil(abs(yPixelPen) - lineHeight), 0.0f);

        // Vertex count
        mVertexCount = (GLuint)vertices.size();

        // Fill into buffer
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 3 * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 2 * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);

        // Restore old setting
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }

    TextFlow::Word TextFlow::calculateWord(std::u16string content, float scale) const
    {
        // Empty word
        Word word;
        word.spVertices = std::shared_ptr<std::vector<glm::vec3> >(new std::vector<glm::vec3>);
        word.spTextureCoordinates = std::shared_ptr<std::vector<glm::vec2> >(new std::vector<glm::vec2>);

        // Fill word with data
        float xPixelPen = 0;
        for (int i = 0; i < content.size(); i++)
        {
            Glyph const * pGlyph = mpFont->getGlyph(mFontSize, content[i]);
            if (pGlyph == NULL)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "TextFlow has character in content not covered by character set");
                continue;
            }

            float yPixelPen = 0 - (scale * (float)(pGlyph->size.y - pGlyph->bearing.y));

            // Vertices for this quad
            glm::vec3 vertexA = glm::vec3(xPixelPen, yPixelPen, 0);
            glm::vec3 vertexB = glm::vec3(xPixelPen + (scale * pGlyph->size.x), yPixelPen, 0);
            glm::vec3 vertexC = glm::vec3(xPixelPen + (scale * pGlyph->size.x), yPixelPen + (scale * pGlyph->size.y), 0);
            glm::vec3 vertexD = glm::vec3(xPixelPen, yPixelPen + (scale * pGlyph->size.y), 0);

            // Texture coordinates for this quad
            glm::vec2 textureCoordinateA = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateB = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateC = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w);
            glm::vec2 textureCoordinateD = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w);

            xPixelPen += scale * pGlyph->advance.x;

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
        word.pixelWidth = xPixelPen;

        return word;
    }

    std::vector<TextFlow::Word> TextFlow::calculateWord(std::u16string content, int maxPixelWidth, float scale) const
    {
        // Calculate word from content
        Word word = calculateWord(content, scale);

        // End of recursion
        if ((content.size() == 1 && word.pixelWidth > maxPixelWidth) || content.empty())
        {
            // Return empty vector as signal of failure
            return std::vector<Word>();
        }
        else if (word.pixelWidth <= maxPixelWidth)
        {
            // If word length is ok, just return it
            return std::vector<Word>(1, word);
        }
        else
        {
            // Word is too wide and content longer than 1, split it!
            int length = (int)content.size();
            int left = length / 2;
            int right = length - left;

            // Combine results from recursive call
            std::vector<Word> leftWord = calculateWord(content.substr(0, left), maxPixelWidth, scale);
            std::vector<Word> rightWord = calculateWord(content.substr(left+1, right), maxPixelWidth, scale);

            // If one or more of both are empty, forget it
            if (leftWord.empty() || rightWord.empty())
            {
                return std::vector<Word>();
            }
            else
            {
                std::vector<Word> words(leftWord);
                words.insert(words.end(), rightWord.begin(), rightWord.end());
                return words;
            }
        }
    }


    bool TextFlow::insertWord(std::vector<TextFlow::Word>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale) const
    {
        // Do nothing if input is empty
        if (rContent.empty())
        {
            return true;
        }

        std::vector<Word> newWords = calculateWord(rContent, maxPixelWidth, scale);

        // Check, whether call was successful
        if (newWords.empty())
        {
            // Was not successful, so return false
            return false;
        }
        else
        {
            // Insert new words and return true
            rWords.insert(rWords.end(), newWords.begin(), newWords.end());
            return true;
        }
    }
}
