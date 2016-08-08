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
        std::u16string content,
		bool overflowHeight) : Text(
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
		mPixelOfSpace = 0.f;
		mOverflowHeight = overflowHeight;

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
		bool renderBackground,
		int xOffset,
		int yOffset) const
    {
        // Calculate y offset because of vertical alignment
        int yAlignmentOffset = 0;
        switch (mVerticalAlignment)
        {
        case TextFlowVerticalAlignment::TOP:
			yAlignmentOffset = 0;
            break;
        case TextFlowVerticalAlignment::CENTER:
			yAlignmentOffset = std::max((mHeight - mFlowHeight) / 2, 0);
            break;
        case TextFlowVerticalAlignment::BOTTOM:
			yAlignmentOffset = std::max((mHeight - mFlowHeight), 0);
            break;
        }

		// Calculate transformation matrix for text flow
		glm::mat4 matrix = glm::mat4(1.0f);
		matrix = glm::translate(
			matrix,
			glm::vec3(
				mX + xOffset,
				mpGUI->getWindowHeight() - (mY + yAlignmentOffset) + yOffset,
				0)); // Change coordinate system and translate to position
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
			glm::mat4 backgroundMatrix = calculateDrawMatrix(
				mpGUI->getWindowWidth(),
				mpGUI->getWindowHeight(),
				mX + ((mWidth - backgroundWidth) / 2) + xOffset,
				mY + yAlignmentOffset - (extraPixels/2) + yOffset,
				backgroundWidth,
				backgroundHeight);
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

	bool TextFlow::getFlowWord(int index, TextFlow::FlowWord& rFlowWord) const
	{
		if (index < mFlowWords.size())
		{
			rFlowWord = mFlowWords.at(index);
			return true;
		}
		else
		{
			return false;
		}
	}

	bool TextFlow::getFlowWord(int x, int y, FlowWord& rFlowWord) const
	{
		for (const auto& rTestFlowWord : mFlowWords)
		{
			for (const auto& rSubFlowWord : rTestFlowWord.subWords)
			{
				// Check whether coordinates inside sub word
				if (x >= rSubFlowWord.x
					&& x < rSubFlowWord.x + rSubFlowWord.width
					&& y >= rSubFlowWord.y
					&& y < rSubFlowWord.y + getLineHeight())
				{
					// If check is true, return flow word
					rFlowWord = rTestFlowWord;
					return true;
				}
			}
		}
		return false;
	}

	void TextFlow::addContent(int index, std::u16string content)
	{
		if (index < mContent.size())
		{
			content.insert(index, content);
			calculateMesh();
		}
	}

    void TextFlow::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates)
    {
		// Reset flow width to get longest line's width of this computation
		mFlowWidth = 0;

		// Clear vector which holds information about words in flow
		mFlowWords.clear();

        // OpenGL setup done in calling method

        // Get size of space character
		mPixelOfSpace = 0;

        Glyph const * pGlyph = mpFont->getGlyph(mFontSize, u' ');
        if (pGlyph == NULL)
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "TextFlow creation does not find space sign in font");
        }
        else
        {
			mPixelOfSpace = mScale * pGlyph->advance.x;
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
			int contentStartIndex = 0;
            while ((pos = rPargraph.find(wordDelimiter)) != std::u16string::npos)
            {
				// Create structure which holds information about word
				mFlowWords.push_back(FlowWord());
				mFlowWords.back().contentIndex = contentStartIndex;

				// Extract current token aka word
                token = rPargraph.substr(0, pos);

				// Erase token from paragraph
                rPargraph.erase(0, pos + wordDelimiter.length());

				// Create vector of fitting words
				std::vector<Word> fitWords;
                failure |= !insertFitWord(fitWords, token, mWidth, mScale);

				// Create as many sub flow words as fit words are added to global words vector
				mFlowWords.back().subWords.resize(fitWords.size());

				// Save index of flow word in vector for addressing by using class
				mFlowWords.back().index = mFlowWords.size() - 1;

				// Set content start index for next run
				contentStartIndex = contentStartIndex + (int)pos + (int)wordDelimiter.length();

				// Insert fitting words to global words vector
				words.insert(words.end(), fitWords.begin(), fitWords.end());
            }

            // Add last token from paragraph as well
			std::vector<Word> fitWords;
            failure |= !insertFitWord(fitWords, rPargraph, mWidth, mScale);
			mFlowWords.push_back(FlowWord());
			mFlowWords.back().contentIndex = contentStartIndex;
			mFlowWords.back().subWords.resize(fitWords.size());
			mFlowWords.back().index = mFlowWords.size() - 1;
			words.insert(words.end(), fitWords.begin(), fitWords.end());

            // Failure appeared, forget it
            if (!failure)
            {
                // Prepare some values
                uint wordIndex = 0;
                bool hasNext = !words.empty();
				int flowWordsIndex = 0;
				int subFlowWordsIndex = 0;

                // Go over lines to write paragraph
                while (hasNext && (abs(yPixelPen) <= mHeight || mOverflowHeight))
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
                                + (((float)line.size()) - 1.0f) * mPixelOfSpace); // Spaces between words
                        }
                    }

                    // If this is last line and after it still words left, replace it by some mark for overflow
                    if (!mOverflowHeight && hasNext && (abs(yPixelPen - lineHeight) > mHeight) && (overflowMark.pixelWidth <= mWidth))
                    {
                        line.clear();
                        wordsPixelWidth = overflowMark.pixelWidth;
                        line.push_back(&overflowMark);
                    }

					// Remember longest line's width
					mFlowWidth = mFlowWidth < ((int) wordsPixelWidth + 1) ? ((int)wordsPixelWidth + 1) : mFlowWidth;

                    // Decide dynamic space for line
                    float dynamicSpace = mPixelOfSpace;
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

						// Save information about layouting of (fit)word to flow words vector
						mFlowWords.at(flowWordsIndex).subWords.at(subFlowWordsIndex).x = (int)xPixelPen;
						mFlowWords.at(flowWordsIndex).subWords.at(subFlowWordsIndex).y = std::ceil(abs(yPixelPen) - lineHeight);
						mFlowWords.at(flowWordsIndex).subWords.at(subFlowWordsIndex).width = (int)line.at(i)->pixelWidth;
						mFlowWords.at(flowWordsIndex).subWords.at(subFlowWordsIndex).lettersXOffsets = line.at(i)->lettersXOffsets;

						// Increment indices
						int subIndexCount = (int)mFlowWords.at(flowWordsIndex).subWords.size();
						subFlowWordsIndex++;
						if (subFlowWordsIndex >= subIndexCount)
						{
							flowWordsIndex++; // next flow word should have at lease one sub word. Otherwise there must have been a failure and this code is not reached
							subFlowWordsIndex = 0;
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
			mFlowHeight = 0;
        }
		else
		{
			// Get height of all lines (yPixelPen is one line to low now)
			mFlowHeight = (int)std::max(std::ceil(abs(yPixelPen) - lineHeight), 0.0f);

			// If overflow allowed, set height to flow height
			if (mOverflowHeight)
			{
				mHeight = mFlowHeight;
			}
		}
    }

    std::vector<Text::Word> TextFlow::calculateFitWord(std::u16string content, int maxPixelWidth, float scale) const
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
