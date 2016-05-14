//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

#include "src/GUI.h"
#include "src/Rendering/AssetManager.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
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
        std::u16string content) : Text(
            pGUI,
            pAssetManager,
            pFont,
            fontSize,
            scale,
            content)
    {
        // Fill members
        mAlignment = alignment;
        mVerticalAlignment = verticalAlignment;
		mFlowWidth = 0;
        mFlowHeight = 0;

        // TransformAndSize has to be called before usage (no calculate mesh is called here)
        mWidth = 0;
        mHeight = 0;
    }

    TextFlow::~TextFlow()
    {
        // Nothing to do here
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
		bool renderBackground) const
    {
        // Calculate y offset because of vertical alignment
        int yOffset = 0;
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

		// Calculate transformation matrix for text flow
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(matrix, glm::vec3(mX, mpGUI->getWindowHeight() - (mY + yOffset), 0)); // Change coordinate system and translate to position
		matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix; // Pixel to world space

		// Draw background
		if (renderBackground)
		{
			// Determine smaller value of width and height
			int value = mWidth < mHeight ? mWidth : mHeight;
			int extraPixels = (int)(((float)value) * TEXT_BACKGROUND_SIZE) - value;

			// Add extra pixels
			int backgroundWidth = extraPixels + mFlowWidth;
			int backgroundHeight = extraPixels + mFlowHeight;

			// Determine which 
			glm::mat4 backgroundMatrix = calculateDrawMatrix(mpGUI->getWindowWidth(), mpGUI->getWindowHeight(), mX + ((mWidth - backgroundWidth) / 2), mY + yOffset - (extraPixels/2), backgroundWidth, backgroundHeight);
			mpBackground->bind();
			mpBackground->getShader()->fillValue("matrix", backgroundMatrix);
			mpBackground->getShader()->fillValue("color", glm::vec4(0.f, 0.f, 0.f, 0.3f));
			mpBackground->getShader()->fillValue("alpha", alpha);
			mpBackground->draw();
		}

        // Bind atlas texture
        if (mScale == 1.0f)
        {
            mpFont->bindAtlasTexture(mFontSize, 1, false);
        }
        else
        {
            mpFont->bindAtlasTexture(mFontSize, 1, true);
        }

		// Bind shader
		mpShader->bind();
		glBindVertexArray(mVertexArrayObject);

        // Fill uniforms
        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("color", color);
        mpShader->fillValue("alpha", alpha);
        mpShader->fillValue("atlas", 1);

        // Draw flow
        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextFlow::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates)
    {
		// Reset flow width to get longest line's width of this computation
		mFlowWidth = 0;

        // OpenGL setup done in calling method

        // Get size of space character
        float pixelOfSpace = 0;

        Glyph const * pGlyph = mpFont->getGlyph(mFontSize, u' ');
        if (pGlyph == NULL)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "TextFlow creation does not find space sign in font");
        }
        else
        {
            pixelOfSpace = mScale * pGlyph->advance.x;
        }

        // Create mark for overflow
        Word overflowMark = calculateWord(TEXT_FLOW_OVERFLOW_MARK, mScale);

        // Get pararaphs separated by \n
        std::vector<std::u16string> paragraphs;
        std::u16string paragraphDelimiter = u"\n";

        // Seperate into paragraphs
        size_t pos = 0;
        std::u16string token;
        while ((pos = streamlinedContent.find(paragraphDelimiter)) != std::u16string::npos)
        {
            token = streamlinedContent.substr(0, pos);
            paragraphs.push_back(token);
            streamlinedContent.erase(0, pos + paragraphDelimiter.length());
        }
        paragraphs.push_back(streamlinedContent); // Last paragraph (paragraphs never empty)

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
                failure |= !insertFitWord(words, token, mWidth, mScale);
            }

            // Add last token from paragraph as well
            failure |= !insertFitWord(words, rPargraph, mWidth, mScale);

            // Failure appeared, forget it
            if (!failure)
            {
                // Prepare some values
                uint wordIndex = 0;
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

					// Remember longest line's width
					mFlowWidth = mFlowWidth < ((int) wordsPixelWidth + 1) ? ((int)wordsPixelWidth + 1) : mFlowWidth;

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
                    for (uint i = 0; i < line.size(); i++)
                    {
                        // Assuming, that the count of vertices and texture coordinates is equal
                        for (uint j = 0; j < line[i]->spVertices->size(); j++)
                        {
                            const glm::vec3& rVertex = line[i]->spVertices->at(j);
                            rVertices.push_back(glm::vec3(rVertex.x + xPixelPen, rVertex.y + yPixelPen, rVertex.z));
                            const glm::vec2& rTextureCoordinate = line[i]->spTextureCoordinates->at(j);
                            rTextureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
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
            rVertices.clear();
            rTextureCoordinates.clear();
        }

        // Get height of all lines (yPixelPen is one line to low now)
        mFlowHeight = (int)std::max(std::ceil(abs(yPixelPen) - lineHeight), 0.0f);
    }

    std::vector<TextFlow::Word> TextFlow::calculateFitWord(std::u16string content, int maxPixelWidth, float scale) const
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
            std::vector<Word> leftWord = calculateFitWord(content.substr(0, left), maxPixelWidth, scale);
            std::vector<Word> rightWord = calculateFitWord(content.substr(left+1, right), maxPixelWidth, scale);

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

    bool TextFlow::insertFitWord(std::vector<TextFlow::Word>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale) const
    {
        // Do nothing if input is empty
        if (rContent.empty())
        {
            return true;
        }

        std::vector<Word> newWords = calculateFitWord(rContent, maxPixelWidth, scale);

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
