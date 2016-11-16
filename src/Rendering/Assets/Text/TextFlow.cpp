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
		bool overflowHeight,
		bool collapseSpaces) : Text(
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
		mCollapseSpaces = collapseSpaces;

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
		matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix; // pixel to world space

		// Draw background
		if (renderBackground)
		{
            drawSimpleBackground(
                mWidth,
                mHeight,
                mFlowWidth,
                mFlowHeight,
                yAlignmentOffset,
                xOffset,
                yOffset,
                alpha);
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

    int TextFlow::getFlowHeight() const
    {
        return mFlowHeight;
    }

    float TextFlow::getPixelWidthOfSpace() const
    {
        return mPixelOfSpace;
    }

    uint TextFlow::getFlowEntityCount() const
    {
        return (uint)mFlowEntities.size();
    }

	std::u16string TextFlow::getContent() const
	{
		return mContent;
	}

    std::u16string TextFlow::getContent(uint index, uint letterCount) const
    {
        return mContent.substr(index, letterCount);
    }

    std::weak_ptr<const FlowEntity> TextFlow::getFlowEntity(uint index) const
    {
        if(index < mFlowEntities.size())
        {
            return mFlowEntities.at(index);
        }
        else
        {
            return std::weak_ptr<const FlowEntity>();
        }
    }

    std::weak_ptr<const FlowEntity> TextFlow::getFlowEntity(int x, int y) const
	{
        for (const auto& rFlowEntity : mFlowEntities)
		{
            for (const auto& rFlowPart : rFlowEntity->mFlowParts)
            {
                // Check whether coordinates are inside flow part of flow entity. Do not use pixel width of complete flow entity because
				// it may be distibuted over multiple lines and the pixel width calculation ignores this possibility. Do is for each flow part
                if(insideRect(rFlowPart->getX(), rFlowPart->getY(), (int)rFlowPart->getPixelWidth(), (int)getLineHeight(), x, y))
                {
                    return rFlowEntity;
                }
            }
		}

        // Fallback
        return std::weak_ptr<const FlowEntity>();
	}

     std::weak_ptr<const FlowEntity> TextFlow::getFlowEntityAndIndices(
            int contentIndex,
            uint& rFlowPartIndex,
            int& rLetterIndex) const
    {
		// Check whether there are flow entities
		if (!mFlowEntities.empty())
		{
			// Special case of front
			if (contentIndex == -1)
			{
				// Set references and return weak pointer
				rFlowPartIndex = 0;
				rLetterIndex = -1;
				return mFlowEntities.front();
			}

			// Check, whether there are enough letters in content for that index
			if (contentIndex >= 0 && contentIndex < mContent.length())
			{
				// Go over flow entities and search index
				uint flowEntityIndex = 0;
				while (
					(flowEntityIndex + 1 < mFlowEntities.size())
					&& (mFlowEntities.at(flowEntityIndex + 1)->getContentStartIndex() <= contentIndex))
				{
					flowEntityIndex++;
				}

				// Found index of flow entity, so assume it as searched flow entity
				std::shared_ptr<const FlowEntity> spFlowEntity = mFlowEntities.at(flowEntityIndex);

				// Fill flow part index and letter index
				uint flowPartIndex = 0;
				int letterIndex = -1;
				if (spFlowEntity->getIndices(contentIndex - spFlowEntity->getContentStartIndex(), flowPartIndex, letterIndex))
				{
					// Set references and return weak pointer
					rFlowPartIndex = flowPartIndex;
					rLetterIndex = letterIndex;
					return spFlowEntity;
				}
			}
		}

        // Fallback
        return std::weak_ptr<const FlowEntity>();
    }

    std::weak_ptr<const FlowEntity> TextFlow::insertContent(uint index, std::u16string content, uint& rFlowPartIndex, int& rLetterIndex)
    {
        // Index has to be advanced by one to be inserted after given index
        uint contentIndex = index + 1;
        if (contentIndex < mContent.size())
		{
            // Insert at given index
            mContent.insert(contentIndex, content);
			calculateMesh();
            return getFlowEntityAndIndices(index + (int)content.length(), rFlowPartIndex, rLetterIndex);
        }
        else if(contentIndex == mContent.size())
        {
            // Append to existing content
            mContent.append(content);
            calculateMesh();
            return getFlowEntityAndIndices(index + (int)content.length(), rFlowPartIndex, rLetterIndex);
        }

        // Fallback
        return std::weak_ptr<const FlowEntity>();
	}

    std::weak_ptr<const FlowEntity> TextFlow::eraseContent(int index, int letterCount, uint& rFlowPartIndex, int& rLetterIndex)
    {
        if(!mContent.empty())
        {
            if (letterCount < 0)
            {
				// Absolute letter count
				int absLetterCount = glm::abs(letterCount);

                // Move index to start of deletion and clamp it
                int clampedIndex = glm::max(0, (index - absLetterCount) + 1); // plus one because letter at original index should be deleted too

                // New letter count
                letterCount = (int)glm::max(0, (index - clampedIndex) + 1);

                // Overwrite index
                index = clampedIndex;
            }

            // Clamp length
            if (index + letterCount >= mContent.size())
            {
                letterCount = (int)glm::max(0, (int)mContent.size() - index);
            }

            // Apply it
            mContent.erase(index, letterCount);

            // Recalulate mesh
            calculateMesh();

			// Calculate index to set (range is -1 .. mContent.size() - 1)
			index = glm::max(-1, glm::min(index - 1, (int)mContent.size() - 1)); // subtract -1 from index because the letter at it has been erased

            // Set indices
            return getFlowEntityAndIndices(index, rFlowPartIndex, rLetterIndex);
        }

        // Fallback
        return std::weak_ptr<const FlowEntity>();
    }

    void TextFlow::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight,
            RenderWordVertices& rVertices)
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
			paragraphs.push_back(streamlinedContent.substr(start, end - start) + u"\n"); // add new line sign for processing at end of paragraph when another paragraph is following
			start = end + 1;
		}
		paragraphs.push_back(streamlinedContent.substr(start));

        // Do not generate text flow mesh when there is a failure
        bool failure = false;

		// Save entities per paragraph and merge later to complete vector saved in members
        std::vector<std::vector<std::shared_ptr<FlowEntity> > > entities(paragraphs.size());

        // Go over paragraphs and draw single lines (pens are in local pixel coordinate system with origin in lower left corner of element)
        float yPixelPen = -lineHeight; // first line should be also inside flow and not on top
		uint globalEntityCount = 0;
		uint paragraphContentOffset = 0; // offset of content start in currently processed paragraph
		for (uint paragraphIndex = 0; paragraphIndex < paragraphs.size(); paragraphIndex++)
		{
			// *** COLLECT ENTITIES FOR EACH PARAGRAPH ***

			// Get entities out of paragraph
            uint letterCount = (uint)paragraphs.at(paragraphIndex).size();
            uint index = 0;
            while(index < letterCount)
			{
                // Determine type
                FlowEntity::Type type = classifyLetter(paragraphs.at(paragraphIndex).at(index));

                // Initialize entity
                std::shared_ptr<FlowEntity> spFlowEntity = std::shared_ptr<FlowEntity>(new FlowEntity);
                spFlowEntity->mType = type;
                spFlowEntity->mContentStartIndex = paragraphContentOffset + index;
                spFlowEntity->mIndex = globalEntityCount;

                switch(spFlowEntity->mType)
                {
                case FlowEntity::Type::Space:
                {              
                    // Determine end index of word in content
                    uint endIndex = index;
                    while (
                        endIndex < letterCount - 1
                        && classifyLetter(paragraphs.at(paragraphIndex).at(endIndex + 1))
                            == FlowEntity::Type::Space)
                    {
                        endIndex++;
                    }

                    // Add flow part for each space
                    for(uint i = index; i <= endIndex; i++)
                    {
                        std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
                        upFlowPart->mPixelWidth = mPixelOfSpace; // just initial value, is updated later in this method
                        spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));
                    }

                    // Go further
                    index = endIndex + 1;
                }
                break;

				case FlowEntity::Type::NewLine:
				{
					// Add empty flow part for the new line
					std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
					upFlowPart->mPixelWidth = 0;
					spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));

					// Go one letter further
					index++;
				}
				break;

                case FlowEntity::Type::Mark:
                {
                    // Add one flow part for the mark
                    std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
                    upFlowPart->mupWord = std::unique_ptr<RenderWord>(
                        new RenderWord(calculateWord(paragraphs.at(paragraphIndex).at(index), mScale)));
                    spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));

                    // Go one letter further
                    index++;
                }
                break;

                case FlowEntity::Type::Word:
                {
                    // Determine end index of word in content
                    uint endIndex = index;
                    while (
                        endIndex < letterCount - 1
                        && classifyLetter(paragraphs.at(paragraphIndex).at(endIndex + 1))
                            == FlowEntity::Type::Word)
                    {
                        endIndex++;
                    }

                    // Create vector of fitting words
                    std::vector<RenderWord> fitWords;
                    failure |= !insertFitWord(fitWords, paragraphs.at(paragraphIndex).substr(index, (endIndex - index) + 1), mWidth, mScale);

                    // Create flow parts words using fit words
                    for (const auto& rFitWord : fitWords)
                    {                       
                        std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
                        upFlowPart->mupWord = std::unique_ptr<RenderWord>(new RenderWord(rFitWord)); // copy fit word
                        spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));
                    }

                    // Go further
                    index = endIndex + 1;
                }
                break;

                default:
                {
                    throwWarning(OperationNotifier::Operation::BUG, "Flow entity type unknown");
                    failure |= true;
                }

                }

                // Push back created entity
                entities[paragraphIndex].push_back(std::move(spFlowEntity));

                // One entity was added
                globalEntityCount++;
			}

			// Update content offset of next paragraph
			paragraphContentOffset += letterCount + 1; // plus one because \n is counted as letter but not saved in paragraph
		}

		if (!failure)
		{
            // *** FILL LINES OF ALL PARAGRAPHS AND DRAW THEM ***

			// Go over paragraphs
			for (uint paragraphIndex = 0; paragraphIndex < entities.size(); paragraphIndex++)
			{
                // *** SETUP LINES OF PARAGRAPH ***

				// Currently processed entity within paragraph
				uint entityIndex = 0;

                // Save index of flow part to draw at front of line
                uint initialFlowPartIndex = 0;

                // Save index of flow part to draw at end of line
                uint endFlowPartIndex = 0;

                // Save index of next flow part which has to be drawn in next line
                uint nextFlowPartIndex = 0;

                // Bool which indicates whether there are still entities to draw left
				bool hasNext = !entities[paragraphIndex].empty();

                // Remember previous filled entity to save position of first flow part of flow entity later at drawing
                FlowEntity const * pPreviousFilledEntity = nullptr;

				// Go over lines
				while (
					hasNext // entities left for new line
					&& (abs(yPixelPen) <= mHeight || mOverflowHeight)) // does not overflow height without permission
				{
                    // Initial flow part index for line
                    initialFlowPartIndex = nextFlowPartIndex;

					// Pixel width of current line
					float lineWidth = 0;

					// Pointer to entities in line
                    std::vector<std::shared_ptr<FlowEntity> > line;

					// Indicator whether is still room for more in the current line
                    bool spaceLeft = lineWidth < mWidth;

                    // Space flow part count for line
                    uint spaceFlowPartCount = 0;

                    // *** SETUP SINGLE LINES OF PARAGRAPH ***

                    // Go over entities to build up line
					while (
						hasNext // entities left to draw into that line
						&& spaceLeft) // whether space is available within line
					{
                        // Whether to add entity to line or not
                        bool addEntityToLine = false;

                        // Save index of currently processed entity's index
                        uint currentEntityIndex = entityIndex;

                        // Fetch pointer to flow entity
                        std::shared_ptr<FlowEntity> spFlowEntity = entities[paragraphIndex].at(currentEntityIndex);

                        // Count of flow parts
                        uint flowPartCount = spFlowEntity->getFlowPartCount();

                        // Get index of flow part to start with
                        uint flowPartIndex = line.empty() ? initialFlowPartIndex : 0;

                        // Go over parts and check for each whether fits into line
                        while (
                            flowPartIndex < flowPartCount // still within entity
                            && (lineWidth +
                                spFlowEntity->mFlowParts.at(flowPartIndex)
                                    ->getPixelWidth()) <= mWidth) // still space left
                        {
                            // New line width including flow part
                            float newLineWidth = lineWidth + spFlowEntity->mFlowParts.at(flowPartIndex)->getPixelWidth();

                            // If currently processed entity is a word and next entity is a mark and both would not fit
                            // into current line, start next line
                            if(spFlowEntity->getType() == FlowEntity::Type::Word // is word
                                && entityIndex + 1 < entities[paragraphIndex].size() // there is next entity
                                && entities[paragraphIndex].at(entityIndex + 1)->getType() == FlowEntity::Type::Mark // next entity would be a mark
                                && newLineWidth + entities[paragraphIndex].at(entityIndex + 1)->getPixelWidth() > mWidth // mark would not fit in same line
                                && !line.empty()) // word would be not alone in line. If alone in line and shifted to next, infinite loop would happen
                            {
                                // Do break to start again in next line
                                break;
                            }
                            //else
                            {
                                addEntityToLine = true; // remember to add entity to line but only one time
                                lineWidth = newLineWidth;
                                endFlowPartIndex = flowPartIndex;
                                flowPartIndex++;
                            }

                            // Spaces must be remembered
                            if(spFlowEntity->getType() == FlowEntity::Type::Space)
                            {
                                spaceFlowPartCount++;
                            }
                        }

                        // Check whether complete block of text is drawn or some flow part is left
                        if(flowPartIndex == flowPartCount)
                        {
                            // Complete block drawn
                            nextFlowPartIndex = 0;
                            entityIndex++;
                        }
                        else
                        {
                            // Flow parts in block are left
                            nextFlowPartIndex = flowPartIndex;
                            spaceLeft = false;
                        }

                        // Add entity to line since at least something of it is drawn
                        if (addEntityToLine) { line.push_back(spFlowEntity); }

                        // Check whether there are entities left
                        hasNext = entityIndex < entities[paragraphIndex].size();
					}

                    // Remember longest line's width
                    mFlowWidth = mFlowWidth < (int)glm::ceil(lineWidth) ? (int)glm::ceil(lineWidth) : mFlowWidth;

					// *** PREPARE DRAWING ***

                    // Get rid of spaces in front and end of line by collapsing them
                    uint collapsedSpaceFlowPartCount = 0;

					// Collapse spaces only when indicated
					if (mCollapseSpaces)
					{
						// Front of line
						if (line.front()->getType() == FlowEntity::Type::Space)
						{
							uint flowPartCount = line.size() == 1 ? endFlowPartIndex + 1 : line.front()->getFlowPartCount();
							uint flowPartIndex = initialFlowPartIndex;
							for (; flowPartIndex < flowPartCount; flowPartIndex++)
							{
								line.front()->mFlowParts.at(flowPartIndex)->mCollapsed = true;
								collapsedSpaceFlowPartCount++;
							}
						}

						// End of line
						if (line.size() > 1 && line.back()->getType() == FlowEntity::Type::Space)
						{
							uint flowPartCount = endFlowPartIndex + 1;
							uint flowPartIndex = line.size() == 1 ? initialFlowPartIndex : 0;
							for (; flowPartIndex < flowPartCount; flowPartIndex++)
							{
								line.back()->mFlowParts.at(flowPartIndex)->mCollapsed = true;
								collapsedSpaceFlowPartCount++;
							}
						}
					}

                    // Width of non space only
                    float nonSpaceWidth = lineWidth - (spaceFlowPartCount * mPixelOfSpace);

                    // Count of space entities which are actually drawn
                    float drawnSpaceFlowPartCount = (float)(spaceFlowPartCount - collapsedSpaceFlowPartCount);

                    // Decide dynamic space for line
					float dynamicSpace = mPixelOfSpace;
					if (
						mAlignment == TextFlowAlignment::JUSTIFY
						&& hasNext // do not use dynamic space for last line
						&& line.size() > 1) // do not use dynamic space when there is only one word
					{
                        dynamicSpace = ((float)mWidth - nonSpaceWidth) / drawnSpaceFlowPartCount;
					}

                    // Calculate the new line width
                    float newLineWidth = nonSpaceWidth + (drawnSpaceFlowPartCount * dynamicSpace);

					// Now decide xOffset for line
					float xOffset = 0;
					if (mAlignment == TextFlowAlignment::RIGHT || mAlignment == TextFlowAlignment::CENTER)
					{
                        xOffset = (float)mWidth - newLineWidth;
						if (mAlignment == TextFlowAlignment::CENTER)
						{
							xOffset /= 2.0f;
						}
					}

					// *** DRAW LINE ***

                    // Integrate entities' geometry to renderable mesh structure
					float xPixelPen = xOffset;
                    for (uint lineIndex = 0; lineIndex < line.size(); lineIndex++)
					{
                        // Fetch pointer to currently drawn entity
                        std::shared_ptr<FlowEntity> spFlowEntity = line.at(lineIndex);

                        // Fill position
                        if(pPreviousFilledEntity != line.at(lineIndex).get()) // fill it only one, so equal to first part's
                        {
                            spFlowEntity->mX = (int)xPixelPen;
                            spFlowEntity->mY = (int)(std::ceil(abs(yPixelPen) - lineHeight));
                        } 

                        // Go over flow parts which are in line
                        uint flowPartCount = lineIndex == line.size() - 1 ? endFlowPartIndex + 1 : line.at(lineIndex)->getFlowPartCount();
                        uint flowPartIndex = lineIndex == 0 ? initialFlowPartIndex : 0;
                        for (; flowPartIndex < flowPartCount; flowPartIndex++)
                        {
                            if(spFlowEntity->getType() == FlowEntity::Type::Space) // space
                            {
                                // Set pixel with of space flow part as fallback
                                if(spFlowEntity->mFlowParts.at(flowPartIndex)->isCollapsed())
                                {
                                    spFlowEntity->mFlowParts.at(flowPartIndex)->mPixelWidth = 0.f;
                                }
                                else
                                {
                                    spFlowEntity->mFlowParts.at(flowPartIndex)->mPixelWidth = dynamicSpace;
                                }
                            }
							else if (spFlowEntity->getType() == FlowEntity::Type::NewLine) // new line
							{
								// do nothing here
							}
                            else // word or mark
                            {
                                // Draw word of flow part
                                if(spFlowEntity->mFlowParts.at(flowPartIndex)->mupWord != nullptr)
                                {
                                    const auto* pWord = line.at(lineIndex)->mFlowParts.at(flowPartIndex)->mupWord.get();

                                    // Push back positions and texture coordinates
                                    for (uint i = 0; i < pWord->spVertices->size(); i++)
                                    {
                                        const std::pair<glm::vec3, glm::vec2>& rVertex = pWord->spVertices->at(i);
                                        rVertices.push_back(
                                            std::make_pair(
                                                glm::vec3(rVertex.first.x + xPixelPen, rVertex.first.y + yPixelPen, rVertex.first.z),
                                                glm::vec2(glm::vec2(rVertex.second.s, rVertex.second.t))));
                                    }
                                }
                                else
                                {
                                    throwWarning(OperationNotifier::Operation::BUG, "TextFlow has nullptr in flow part which should not exist");
                                    failure |= true;
                                }
                            }

							// Save position in flow part
							spFlowEntity->mFlowParts.at(flowPartIndex)->mX = (int)xPixelPen;
							spFlowEntity->mFlowParts.at(flowPartIndex)->mY = (int)(std::ceil(abs(yPixelPen) - lineHeight));

                            // Advance xPen
                            xPixelPen += spFlowEntity->mFlowParts.at(flowPartIndex)->getPixelWidth();

                        } // end of flow part iteration

                        // Update previous filled entity
                        pPreviousFilledEntity = spFlowEntity.get();

                    } // end of line

					// Advance yPen if there is a next line
					if (hasNext)
					{
						yPixelPen -= lineHeight;
					}

                } // end of paragraph

				// If there are paragraphs left, move yPen by one line
				if ((int)paragraphIndex < (int)entities.size() - 1)
				{
					yPixelPen -= lineHeight;
				}
			}
        }

        // If failure appeared, clean up
        if (failure)
        {
            // Reset everything
            rVertices.clear();
			mFlowWidth = 0;
			mFlowHeight = 0;
			mFlowEntities.clear();
        }
		else
		{
			// Get height of all lines
			mFlowHeight = (int)glm::abs(yPixelPen);

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

    std::vector<RenderWord> TextFlow::calculateFitWord(std::u16string content, int maxPixelWidth, float scale) const
    {
        // Calculate word from content
        RenderWord word = calculateWord(content, scale);

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
            std::vector<RenderWord> leftWord = calculateFitWord(content.substr(0, left), maxPixelWidth, scale);
            std::vector<RenderWord> rightWord = calculateFitWord(content.substr(left, right), maxPixelWidth, scale);

            // If one or more of both are empty, forget it
            if (leftWord.empty() || rightWord.empty())
            {
				return {};
            }
            else
            {
                std::vector<RenderWord> words(leftWord);
                words.insert(words.end(), rightWord.begin(), rightWord.end());
                return words;
            }
        }
    }

    bool TextFlow::insertFitWord(std::vector<RenderWord>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale) const
    {
        // Do nothing if input is empty
        if (rContent.empty())
        {
            return true;
        }

        std::vector<RenderWord> newWords = calculateFitWord(rContent, maxPixelWidth, scale);

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

    FlowEntity::Type TextFlow::classifyLetter(const char16_t& rLetter) const
    {
        switch(rLetter)
        {
            case u' '	: return FlowEntity::Type::Space;
			case u'\n'	: return FlowEntity::Type::NewLine;
            case u'.'	: return FlowEntity::Type::Mark;
            case u':'	: return FlowEntity::Type::Mark;
            case u','	: return FlowEntity::Type::Mark;
            case u';'	: return FlowEntity::Type::Mark;
            case u'-'	: return FlowEntity::Type::Mark;
            case u'_'	: return FlowEntity::Type::Mark;
            case u'?'	: return FlowEntity::Type::Mark;
            case u'!'	: return FlowEntity::Type::Mark;
            case u'/'	: return FlowEntity::Type::Mark;
            case u'\\'	: return FlowEntity::Type::Mark;
            case u'"'	: return FlowEntity::Type::Mark;
            default		: return FlowEntity::Type::Word;
        }
    }
}
