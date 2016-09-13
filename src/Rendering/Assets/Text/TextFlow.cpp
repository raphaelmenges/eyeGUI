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

		// Save entities per paragraph and merge later to complete vector saved in members
		std::vector<std::vector<std::unique_ptr<FlowEntity> > > entities(paragraphs.size());

        // Go over paragraphs and draw single lines (pens are in local pixel coordinate system with origin in lower left corner of element)
        float yPixelPen = -lineHeight; // first line should be also inside flow and not on top
		uint globalEntityCount = 0;
		for (uint paragraphIndex = 0; paragraphIndex < paragraphs.size(); paragraphIndex++)
		{
			// *** COLLECT ENTITIES FOR EACH PARAGRAPH ***

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
					// Word
					while (endIndex < letterCount - 1 && paragraphs.at(paragraphIndex).at(endIndex + 1) != u' ')
					{
						endIndex++;
					}

					// Create vector of fitting words
					std::vector<Word> fitWords;
					failure |= !insertFitWord(fitWords, paragraphs.at(paragraphIndex).substr(index, (endIndex - index) + 1), mWidth, mScale);

					// Create entity
					std::unique_ptr<FlowWord> upFlowWord = std::unique_ptr<FlowWord>(new FlowWord);

					// Create subwords using fit words
					for (const auto& rFitWord : fitWords)
					{
						std::unique_ptr<SubFlowWord> upSubFlowWord = std::unique_ptr<SubFlowWord>(new SubFlowWord);
						upSubFlowWord->upWord = std::unique_ptr<Word>(new Word(rFitWord)); // copy fit word
						upFlowWord->subWords.push_back(std::move(upSubFlowWord));
					}

					// Push back entity
					entities[paragraphIndex].push_back(std::move(upFlowWord));
				}

				// Set up for all kind of entities within text
				entities[paragraphIndex].back()->contentStartIndex = index;
				entities[paragraphIndex].back()->index = globalEntityCount;

				// Prepare next loop
				globalEntityCount++;
                index = endIndex; // incremented by for loop, too
			}
		}

		// Go over entities and draw them
		if (!failure)
		{
            // *** FILL LINES OF ALL PARAGRAPHS AND DRAW THEM ***

			// Go over paragraphs
			for (uint paragraphIndex = 0; paragraphIndex < entities.size(); paragraphIndex++)
			{
                // *** SETUP LINES OF PARAGRAPH ***

				// Currently processed entity within paragraph
				uint entityIndex = 0;

				// Used to know at drawing in which part of first entity to start
				uint initialPartIndex = 0;

                // Index of next part of entity to draw
                uint nextPartIndex = 0;

				// Bool which indicates whether there are still entities to draw
				bool hasNext = !entities[paragraphIndex].empty();

				// Go over lines
				while (
					hasNext // entities left for new line
					&& (abs(yPixelPen) <= mHeight || mOverflowHeight)) // does not overflow height without permission
				{
					// Initial part index for drawing
                    initialPartIndex = nextPartIndex; // either last part is empty since last line has drawn complete word or it holds the last drawn index

					// Pixel width of current line
					float lineWidth = 0;

					// Pointer to entities in line
					std::vector<FlowEntity*> line;

					// Count of space letters, used for drawing
					uint spaceLetterCount = 0;

					// Indicator whether is still room for more in the current line
					bool spaceLeft = true;

					// Current index within current entity [0..partCount]
                    uint partIndex = initialPartIndex;

                    // Count of parts of current entity
                    uint partCount = 0;

                    // *** SETUP SINGLE LINE IN PARAGRAPH ***

					// Go over parts to build up line
					while (
						hasNext // entities left to draw into that line
						&& spaceLeft) // whether space is available within line
					{
						// Whether to add entity to line or not
						bool addEntityToLine = false;

						// Check which entity is given
						FlowEntityType type = entities[paragraphIndex].at(entityIndex)->getType();

						// Decide what to do
						if (type == FlowEntityType::Space)
						{
							FlowSpace const * pFlowSpace = dynamic_cast<FlowSpace const *>(entities[paragraphIndex].at(entityIndex).get());
							partCount = pFlowSpace->count;

							// Go over parts and check for each whether fits into line
							while (
								partIndex < partCount // still within entity
								&& (lineWidth + mPixelOfSpace) <= mWidth) // still space left
							{
								addEntityToLine = true;
								lineWidth += mPixelOfSpace;
								partIndex++;
								spaceLetterCount++;
							}

						}
						else // Word
						{
							FlowWord const * pFlowWord = dynamic_cast<FlowWord const *>(entities[paragraphIndex].at(entityIndex).get());
							partCount = pFlowWord->subWords.size();

							// Go over parts and check for each whether fits into line
							while (
								partIndex < partCount // still within entity
								&& (lineWidth + pFlowWord->subWords.at(partIndex)->upWord->pixelWidth) <= mWidth) // still space left
							{
								addEntityToLine = true;
								lineWidth += pFlowWord->subWords.at(partIndex)->upWord->pixelWidth;
								partIndex++;
							}
						}

						// Add entity to line since at least one part is drawn
						if (addEntityToLine) { line.push_back(entities[paragraphIndex].at(entityIndex).get()); }

						// Check whether entity was completely drawn
                        if (partIndex >= partCount) // should be equal when completely drawn
						{
							// Entity completely drawn, go on with next one
							partIndex = 0;
							entityIndex++;

							// Check whether there are entities left
							hasNext = entityIndex < entities[paragraphIndex].size();
						}
						else
						{
							// Entity not completely drawn
							spaceLeft = false;

                            // Remember where to go on in next line
                            nextPartIndex = partIndex;
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
						float wordsWidth = lineWidth - (spaceLetterCount * mPixelOfSpace);
						dynamicSpace = ((float)mWidth - wordsWidth) / (float)spaceLetterCount;
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
					for (uint lineEntity = 0; lineEntity < line.size(); lineEntity++)
					{
						// Check what entity is given
						FlowEntityType type = line.at(lineEntity)->getType();

						// Decide what to do
						if (type == FlowEntityType::Space)
						{
							FlowSpace* pFlowSpace = dynamic_cast<FlowSpace*>(line.at(lineEntity));

							// Just advance xPen
							uint spaceCount = pFlowSpace->count;
                            if (lineEntity == line.size() - 1 // last entity in line
                                && hasNext) // using next part index make only sense when there is something after
							{
                                spaceCount = nextPartIndex;
							}
							uint spaceIndex = lineEntity == 0 ? initialPartIndex : 0;
							for (; spaceIndex < spaceCount; spaceIndex++)
							{
								xPixelPen += dynamicSpace;
							}
						}
						else // Word
						{
							FlowWord* pFlowWord = dynamic_cast<FlowWord*>(line.at(lineEntity));

							// Go over sub words which are part of line
							uint subWordCount = pFlowWord->subWords.size();
                            if (lineEntity == line.size() - 1 // last entity in line
                                && hasNext) // using next part index make only sense when there is something after
							{
                                subWordCount = nextPartIndex;
							}
							uint subWordIndex = lineEntity == 0 ? initialPartIndex : 0;
							for (; subWordIndex < subWordCount; subWordIndex++)
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
            // Reset everything
            rVertices.clear();
            rTextureCoordinates.clear();
			mFlowWidth = 0;
			mFlowHeight = 0;
			mFlowEntities.clear();
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
			mFlowEntities.clear();
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
            std::vector<Word> rightWord = calculateFitWord(content.substr(left, right), maxPixelWidth, scale);

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
