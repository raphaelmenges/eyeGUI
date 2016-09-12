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

// TODO: testing
#include <iostream>

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
		/*
		if (index < mFlowWords.size())
		{
			rFlowWord = mFlowWords.at(index);
			return true;
		}
		else
		{
			return false;
		}
		*/
		return false; // TODO: temporarly added
	}

	bool TextFlow::getFlowWord(int x, int y, FlowWord& rFlowWord) const
	{
		/*
		for (const auto& rTestFlowWord : mFlowWords)
		{
            for (const auto& rSubWord : rTestFlowWord.subWords)
			{
				// Check whether coordinates inside sub word
                if (x >= rSubWord.x
                    && x < rSubWord.x + rSubWord.width
                    && y >= rSubWord.y
                    && y < rSubWord.y + getLineHeight())
				{
					// If check is true, return flow word
                    rFlowWord = rTestFlowWord;
					return true;
				}
			}
		}
		*/
		return false;
	}

    bool TextFlow::getFlowWordAndIndices(int contentIndex, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex) const
    {
		/*
		// Special case of front
		if (contentIndex == -1 && !mFlowWords.empty())
		{
			rFlowWord = mFlowWords.front();
			rSubWordIndex = 0;
			rLetterIndex = -1;
			return true;
		}

        // Check, whether there are enough letters in content for that index
        if(contentIndex >= 0 && contentIndex < mContent.length())
        {
            // Go over flow words and search index
            int flowIndex = 0;
            while ((flowIndex + 1 < (int)mFlowWords.size()) && (mFlowWords.at(flowIndex + 1).contentStartIndex <= contentIndex))
			{
				flowIndex++;
			}

            // Found index, so assume it as searched flow word
            FlowWord flowWord = mFlowWords.at(flowIndex);

            // Fill sub word index and letter index
            int subWordIndex = 0;
            int letterIndex = 0;
            if(flowWord.getIndices(contentIndex - flowWord.contentStartIndex, subWordIndex, letterIndex))
            {
                // Set references and return true
                rFlowWord = flowWord;
                rSubWordIndex = subWordIndex;
                rLetterIndex = letterIndex;
                return true;
            }
            else
            {
                // Index was not found within word, it is space behind that and position before next
                if(flowIndex + 1 < (int)mFlowWords.size())
                {
                    // Set references and return true
                    rFlowWord = mFlowWords.at(flowIndex + 1);
                    rSubWordIndex = 0;
                    rLetterIndex = -1;
                    return true;
                }
            }
        }
		*/
        return false;
    }

    bool TextFlow::insertContent(int index, std::u16string content, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex)
    {
		/*
        // Index has to be advanced by one to be insert after given index
        int contentIndex = index + 1;
        if (contentIndex < (int)mContent.size())
		{
            // Insert at given index
            mContent.insert(contentIndex, content);
			calculateMesh();
            return getFlowWordAndIndices(index + content.length(), rFlowWord, rSubWordIndex, rLetterIndex);
        }
        else if(contentIndex == (int)mContent.size())
        {
            // Append to existing content
            mContent.append(content);
            calculateMesh();
            return getFlowWordAndIndices(index + content.length(), rFlowWord, rSubWordIndex, rLetterIndex);
        }
		*/
        return false;
	}

    bool TextFlow::eraseContent(int index, int letterCount, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex)
    {
		/*
		if (letterCount < 0)
		{
			// Move index
			index -= letterCount;

			// Clamp index
			int clampedIndex = glm::max(index, 0);

			// New letter count
			letterCount = index - clampedIndex;

			// Overwrite index
			index = clampedIndex;
		}

		// Clamp length
		int endIndex = index + letterCount;
		if (endIndex >= (int)mContent.size())
		{
			letterCount -= (endIndex - ((int)mContent.size() - 1));
		}

		// Apply it
		mContent.erase(index, letterCount);

		// Recalulate mesh
		calculateMesh();

		// Set indices
		return getFlowWordAndIndices(index, rFlowWord, rSubWordIndex, rLetterIndex);
		*/
		return false; // TODO: temporarly added
    }

	std::u16string TextFlow::getContent(int index, int letterCount) const
	{
		// TODO: what if letter count is negative?
		return mContent.substr(index, letterCount);
	}

    void TextFlow::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates)
    {
		// *** PREPARATION ***

		// OpenGL setup done in calling method

		// Reset flow width to get longest line's width of this computation
		mFlowWidth = 0;

		// Clear vector which holds information about entities in flow
		mFlowEntities.clear();

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

		// *** COLLECT PARAGRAPHS ***

        // Get pararaphs separated by \n
        std::vector<std::u16string> paragraphs;
        std::u16string paragraphDelimiter = u"\n";

        // Seperate into paragraphs
		std::size_t start = 0, end = 0;
		while ((end = streamlinedContent.find(paragraphDelimiter, start)) != std::string::npos) {
			paragraphs.push_back(streamlinedContent.substr(start, end - start));
			start = end + 1;
		}
		paragraphs.push_back(streamlinedContent.substr(start));

        // Do not generate text flow mesh when there is a failure
        bool failure = false;

		// Save entities per paragraph and merge later
		std::vector<std::vector<std::unique_ptr<FlowEntity> > > entities(paragraphs.size());

        // Go over paragraphs and draw single lines (pens are in local pixel coordinate system with origin in lower left corner of element)
        float yPixelPen = -lineHeight; // first line should be also inside flow and not on top
		uint globalEntityCount = 0;
		for (uint paragraphIndex = 0; paragraphIndex < paragraphs.size(); paragraphIndex++)
		{
			// *** COLLECT ENTITIES ***

			// Get entities out of paragraph
			uint letterCount = paragraphs.at(paragraphIndex).size();
			for (uint index = 0; index < letterCount; index++)
			{
				uint endIndex = index;

				// Decide which entity to create
				if (paragraphs.at(paragraphIndex).at(index) == u' ')
				{
					// Space
					while (endIndex < letterCount - 1 && paragraphs.at(paragraphIndex).at(endIndex + 1) == u' ')
					{
						endIndex++;
					}

					// Create entity
					std::unique_ptr<FlowSpace> upFlowSpace = std::unique_ptr<FlowSpace>(new FlowSpace);
					upFlowSpace->count = (endIndex - index) + 1;
					upFlowSpace->contentStartIndex = index;

					// Push back entity
					entities[paragraphIndex].push_back(std::move(upFlowSpace));
				}
				else
				{
					// No space...
					while (endIndex < letterCount - 1 && paragraphs.at(paragraphIndex).at(endIndex + 1) != u' ')
					{
						endIndex++;
					}

					// Create vector of fitting words
					std::vector<Word> fitWords;
					failure |= !insertFitWord(fitWords, paragraphs.at(paragraphIndex).substr(index, (endIndex - index) + 1), mWidth, mScale);

					// Create entity
					std::unique_ptr<FlowWord> upFlowWord = std::unique_ptr<FlowWord>(new FlowWord);

					// Create subwords
					for (const auto& rFitWord : fitWords)
					{
						std::unique_ptr<SubFlowWord> upSubFlowWord = std::unique_ptr<SubFlowWord>(new SubFlowWord);
						upSubFlowWord->upWord = std::unique_ptr<Word>(new Word(rFitWord));
						upFlowWord->subWords.push_back(std::move(upSubFlowWord));
					}

					// Push back entity
					entities[paragraphIndex].push_back(std::move(upFlowWord));
				}

				// Set index within vector
				entities[paragraphIndex].back()->contentStartIndex = index;
				entities[paragraphIndex].back()->index = globalEntityCount;
				globalEntityCount++;
				index = endIndex; // increment by for loop, too
			}
		}

		// Go over entities and draw them
		if (!failure)
		{
			// Go over paragraphs
			for (uint paragraphIndex = 0; paragraphIndex < entities.size(); paragraphIndex++)
			{
				// *** SETUP LINES OF PARAGRAPH ***

				// Prepare some values
				uint entityIndex = 0;

				// Remember where last line stopped
				uint innerEnd = 0; // either count for space or sub word index within word. Do not access that but one before!
				uint innerStart = 0; // same as above but holds information where to start for that line

				// Go over lines to write paragraph
				bool hasNext = !entities[paragraphIndex].empty();
				while (
					hasNext // entities left for new line
					&& (abs(yPixelPen) <= mHeight || mOverflowHeight)) // does not overflow height without permission
				{
					// *** SETUP SINGLE LINE IN PARAGRAPH ***

					// Remember where that lines started since it is the end of the last
					innerStart = innerEnd;

					// Collect words in one line
					float lineWidth = 0;
					std::vector<FlowEntity*> line;
					uint spaceCount = 0;
					bool spaceLeft = true;

					// Still entities in the paragraph and enough space? Fill into current line!
					while (
						hasNext // entities left to draw into that line
						&& spaceLeft) // whether space is available
					{
						// Check what entity is given
						FlowEntityType type = entities[paragraphIndex].at(entityIndex)->getType();

						// Count of internal (either space count or subword count)
						uint innerCount = 0;

						// Bool whether to add entity
						bool addEntity = false;

						// Decide what to do
						if (type == FlowEntityType::Space)
						{
							// Add as many pixels as the spaces would need
							innerCount = dynamic_cast<FlowSpace const *>(entities[paragraphIndex].at(entityIndex).get())->count;
							for (uint i = innerEnd; i < innerCount; i++)
							{
								// Decide whether it fits in line or not
								float newLineWidth = lineWidth + mPixelOfSpace;
								if (newLineWidth <= mWidth)
								{
									lineWidth = newLineWidth;
									spaceCount++; // remember how many spaces are in that line
									addEntity = true; // at least one space is in line
								}
								else
								{
									innerEnd = i;
									spaceLeft = false;
									break;
								}
							}
						}
						else // Word
						{
							// Add as many words as available
							FlowWord const * pFlowWord = dynamic_cast<FlowWord const *>(entities[paragraphIndex].at(entityIndex).get());
							innerCount = pFlowWord->subWords.size();
							for (uint i = innerEnd; i < innerCount; i++)
							{
								// Decide whether it fits in line or not
								float newLineWidth = lineWidth + pFlowWord->subWords.at(i)->upWord->pixelWidth;
								if (newLineWidth <= mWidth)
								{
									lineWidth = newLineWidth;
									addEntity = true; // at least one word is in line
								}
								else
								{
									innerEnd = i;
									spaceLeft = false;
									break;
								}
							}
						}

						if (addEntity)
						{
							// Add flow entity to line
							line.push_back(entities[paragraphIndex].at(entityIndex).get());

							// Entity is completely drawn, next one please
							if (innerEnd + 1 == innerCount)
							{
								entityIndex++;
								innerEnd = 0; // reset end since next entity is about to be used

								// Decide whether has next
								hasNext = entities[paragraphIndex].size() > entityIndex;
							}
						}
					}

					// For last word in last line, set inner end counter to complete count since it is used later for index bounding
					if (!hasNext)
					{
						FlowEntityType type = entities[paragraphIndex].back()->getType();
						if (type == FlowEntityType::Space)
						{
							innerEnd = dynamic_cast<FlowSpace const *>(entities[paragraphIndex].back().get())->count;
						}
						else
						{
							innerEnd = dynamic_cast<FlowWord const *>(entities[paragraphIndex].back().get())->subWords.size();
						}

					}
					// *** PREPARE DRAWING ***

					// Remember longest line's width
					mFlowWidth = mFlowWidth < glm::ceil(lineWidth) ? glm::ceil(lineWidth) : mFlowWidth;

					// Decide dynamic space for line
					float dynamicSpace = mPixelOfSpace;
					if (
						mAlignment == TextFlowAlignment::JUSTIFY
						&& hasNext // do not use dynamic space for last line
						&& line.size() > 1) // do not use dynamic space when there is only one word
					{
						// For justify, do something dynamic
						float wordWidth = lineWidth - (spaceCount * mPixelOfSpace);
						dynamicSpace = (lineWidth - wordWidth) / (float)spaceCount;
					}

					// Now decide xOffset for line
					float xOffset = 0;
					if (mAlignment == TextFlowAlignment::RIGHT || mAlignment == TextFlowAlignment::CENTER)
					{
						xOffset = (float)mWidth - lineWidth;
						if (mAlignment == TextFlowAlignment::CENTER)
						{
							xOffset /= 2.0f;
						}
					}

					// *** DRAW LINE ***

					// Combine word geometry to one renderable line
					float xPixelPen = xOffset;
					for (uint entityIndex = 0; entityIndex < line.size(); entityIndex++)
					{
						// Check what entity is given
						FlowEntityType type = line.at(entityIndex)->getType();

						// Decide what to do
						if (type == FlowEntityType::Space)
						{
							// Convert to flow space
							FlowSpace* pFlowSpace = dynamic_cast<FlowSpace*>(line.at(entityIndex));

							// Advance xPen
							uint endSpaceIndex = entityIndex == line.size() - 1 ? innerEnd : pFlowSpace->count;
							uint spaceIndex = entityIndex == 0 ? innerStart : 0;
							for (; spaceIndex < endSpaceIndex; spaceIndex++)
							{
								xPixelPen += dynamicSpace;
							}
						}
						else // Word
						{
							// Convert to flow word
							FlowWord* pFlowWord = dynamic_cast<FlowWord*>(line.at(entityIndex));

							// Go over sub words which are part of line
							uint endSubWordIndex = entityIndex == line.size() - 1 ? innerEnd : pFlowWord->subWords.size();
							uint subWordIndex = entityIndex == 0 ? innerStart : 0;
							for (; subWordIndex < endSubWordIndex; subWordIndex++)
							{
								// Fetch pointer to word
								const auto* pWord = pFlowWord->subWords.at(subWordIndex)->upWord.get();

								// Push back vertices and texture coordiantes
								for (uint i = 0; i < pWord->spVertices->size(); i++)
								{
									const glm::vec3& rVertex = pWord->spVertices->at(i);
									rVertices.push_back(glm::vec3(rVertex.x + xPixelPen, rVertex.y + yPixelPen, rVertex.z));
									const glm::vec2& rTextureCoordinate = pWord->spTextureCoordinates->at(i);
									rTextureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
								}

								// Save position stuff in subword
								pFlowWord->subWords.at(subWordIndex)->x = (int)xPixelPen;
								pFlowWord->subWords.at(subWordIndex)->y = std::ceil(abs(yPixelPen) - lineHeight);

								// Advance xPen
								xPixelPen += pWord->pixelWidth;
							}
						}
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

			// Transfer all entities to member
			for (auto& entitiesOfParagraph : entities)
			{
				mFlowEntities.insert(mFlowEntities.end(),
					std::make_move_iterator(entitiesOfParagraph.begin()),
					std::make_move_iterator(entitiesOfParagraph.end()));
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
			return {};
        }
        else if (word.pixelWidth <= maxPixelWidth)
        {
            // If word length is ok, just return it
			return { word };
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
				return {};
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
