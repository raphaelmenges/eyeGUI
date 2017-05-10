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

// TODO TESTING
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

    std::u16string TextFlow::getContent(uint contentIndex, uint letterCount) const
    {
        return mContent.substr(contentIndex, letterCount);
    }

    std::weak_ptr<const FlowEntity> TextFlow::getFlowEntity(uint entityIndex) const
    {
        if(entityIndex < mFlowEntities.size())
        {
            return mFlowEntities.at(entityIndex);
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
            if (contentIndex >= 0 && contentIndex < (int)mContent.length())
			{
				// Go over flow entities and search index
				uint flowEntityIndex = 0;
				while (
                    (flowEntityIndex + 1 < (uint)mFlowEntities.size())
                    && (mFlowEntities.at(flowEntityIndex + 1)->getContentStartIndex() <= (uint)contentIndex))
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

    std::weak_ptr<const FlowEntity> TextFlow::insertContent(int contentIndex, std::u16string content, uint& rFlowPartIndex, int& rLetterIndex)
    {
        // Index has to be advanced by one to be inserted after given index
        uint index = contentIndex + 1;
        if (index < mContent.size())
		{
            // Insert at given index
            mContent.insert(index, content);
			calculateMesh();
            return getFlowEntityAndIndices(contentIndex + (int)content.length(), rFlowPartIndex, rLetterIndex);
        }
        else if(index == mContent.size())
        {
            // Append to existing content
            mContent.append(content);
            calculateMesh();
            return getFlowEntityAndIndices(contentIndex + (int)content.length(), rFlowPartIndex, rLetterIndex);
        }

        // Fallback
        return std::weak_ptr<const FlowEntity>();
	}

    std::weak_ptr<const FlowEntity> TextFlow::eraseContent(int contentIndex, int letterCount, uint& rFlowPartIndex, int& rLetterIndex)
    {
        if(!mContent.empty())
        {
            if (letterCount < 0)
            {
				// Absolute letter count
				int absLetterCount = glm::abs(letterCount);

                // Move index to start of deletion and clamp it
                int clampedIndex = glm::max(0, (contentIndex - absLetterCount) + 1); // plus one because letter at original index should be deleted too

                // New letter count
                letterCount = (int)glm::max(0, (contentIndex - clampedIndex) + 1);

                // Overwrite index
				contentIndex = clampedIndex;
            }

            // Clamp length
            if (contentIndex + letterCount >= (int)mContent.size())
            {
                letterCount = (int)glm::max(0, (int)mContent.size() - contentIndex);
            }

            // Apply it
            mContent.erase(contentIndex, letterCount);

            // Recalulate mesh
            calculateMesh();

			// Calculate index to set (range is -1 .. mContent.size() - 1)
			contentIndex = glm::max(-1, glm::min(contentIndex - 1, (int)mContent.size() - 1)); // subtract -1 from index because the letter at it has been erased

            // Set indices
            return getFlowEntityAndIndices(contentIndex, rFlowPartIndex, rLetterIndex);
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

		// Clear entity vector
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

        // Do not generate text flow mesh when there is a failure
        bool failure = false;

        // Go over lines and draw single ones (pens are in local pixel coordinate system with origin in lower left corner of element)
        float yPixelPen = -lineHeight; // first line should be also inside flow and not on top

		// *** COLLECT ENTITIES REPRESENTING TEXT ***

        // Get entities out of content
        uint letterCount = (uint)streamlinedContent.size();
        uint index = 0;
		bool globalRightToLeft = false;
		
		// Determine initial text direction
		for (const auto& character : streamlinedContent)
		{
			CharacterDirection direction = mpFont->getCharacterDirection(character);
			if (direction == CharacterDirection::LeftToRight)
			{
				globalRightToLeft = false;
				break;
			}
			else if (direction == CharacterDirection::RightToLeft)
			{
				globalRightToLeft = true;
				break;
			}
		}

		// Inner text direction, which can change depending on letters in text
		bool partRightToLeft = globalRightToLeft;

		// Define a lambda to check for direction change
		const std::function<bool(bool&, char16_t)> directionChange = [this](bool& rRightToLeft, char16_t character)
		{
			CharacterDirection direction = this->mpFont->getCharacterDirection(character);
			bool directionChanged = false;
			if (direction == CharacterDirection::LeftToRight)
			{
				if (rRightToLeft)
				{
					directionChanged = true;
					rRightToLeft = false;
				}
			}
			else if (direction == CharacterDirection::RightToLeft)
			{
				if (!rRightToLeft)
				{
					directionChanged = true;
					rRightToLeft = true;
				}
			}
			return directionChanged;
		};

		// Go over streamlined content
        while(index < letterCount)
		{
            // Determine type
            FlowEntity::Type type = classifyLetter(streamlinedContent.at(index));

			// Determine direction for character at current index (reference of partRightToLeft overriden)
			directionChange(partRightToLeft, streamlinedContent.at(index)); // return value of no interest here, as already starting a new entity

            // Initialize entity
            std::shared_ptr<FlowEntity> spFlowEntity = std::shared_ptr<FlowEntity>(new FlowEntity);
            spFlowEntity->mType = type;
            spFlowEntity->mContentStartIndex = index;
            spFlowEntity->mIndex = (uint)mFlowEntities.size();
			spFlowEntity->rightToLeft = partRightToLeft;

            switch(spFlowEntity->mType)
            {
            case FlowEntity::Type::Space: // cannot change direction
            {
                // Determine end index of word in content
                uint endIndex = index;
                while (
                    endIndex < letterCount - 1
                    && classifyLetter(streamlinedContent.at(endIndex + 1))
                        == FlowEntity::Type::Space) // no check for direction change, as space cannot trigger one
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

			case FlowEntity::Type::NewLine: // cannot change direction
			{
				// Add empty flow part for the new line
				std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
				upFlowPart->mPixelWidth = 0;
				spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));

				// Go one letter further
				index++;
			}
			break;

			// TODO: For BIDI rendering, no wrapping parentheses around directional different text are possible, as the opening one belongs to the surounding direction and closing to the inner
            case FlowEntity::Type::Mark: // cannot change direction
            {
                // Add one flow part for the mark
                std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
                upFlowPart->mupRenderWord = std::unique_ptr<RenderWord>(
                    new RenderWord(calculateWord(streamlinedContent.at(index), mScale, partRightToLeft)));
                spFlowEntity->mFlowParts.push_back(std::move(upFlowPart));

                // Go one letter further
                index++;
            }
            break;

            case FlowEntity::Type::Word: // can change direction
            {
                // Determine end index of word in content
                uint endIndex = index;
				bool tmpRightToLeft = partRightToLeft; // following call of direction change might overwrite value, which is not desired
                while (
                    endIndex < letterCount - 1 // letters in content left
					&& !directionChange(tmpRightToLeft, streamlinedContent.at(endIndex + 1)) // direction does not change, otherwise end word and start next one
                    && classifyLetter(streamlinedContent.at(endIndex + 1))
                        == FlowEntity::Type::Word) // classification as word
                {
                    endIndex++;
                }

                // Create vector of fitting words
                std::vector<RenderWord> fitWords;
                failure |= !insertFitWord(fitWords, streamlinedContent.substr(index, (endIndex - index) + 1), mWidth, mScale, partRightToLeft);

                // Create flow parts words using fit words
                for (const auto& rFitWord : fitWords)
                {                       
                    std::unique_ptr<FlowPart> upFlowPart = std::unique_ptr<FlowPart>(new FlowPart);
                    upFlowPart->mupRenderWord = std::unique_ptr<RenderWord>(new RenderWord(rFitWord)); // copy fit word
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
            mFlowEntities.push_back(std::move(spFlowEntity));
		}

		if (!failure)
		{
            // *** SETUP LINES ***

			// Currently processed entity
			uint entityIndex = 0;

            // Save index of flow part to draw at front of line
            uint initialFlowPartIndex = 0;

            // Save index of flow part to draw at end of line
            uint endFlowPartIndex = 0;

            // Save index of next flow part which has to be drawn in next line
            uint nextFlowPartIndex = 0;

            // Bool which indicates whether there are still entities to draw left
			bool hasNext = !mFlowEntities.empty();

            // Remember previous filled entity to save position of first flow part of flow entity later at drawing
            FlowEntity const * pPreviousFilledEntity = nullptr;

			// Create the lines
			while (
				hasNext // entities left for new line
				&& (glm::abs(yPixelPen) <= mHeight || mOverflowHeight)) // does not overflow height without permission
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

				// Mark for end of paragraph. A new line is then started
				bool endOfParagraph = false;

                // *** SETUP SINGLE LINE ***

                // Go over remaining entities to build up line
				while (
					hasNext // entities left to draw into that line
					&& spaceLeft // whether space is available within line
					&& !endOfParagraph) // stop single line when end of paragraph reached
				{
                    // Whether to add entity to line or not
                    bool addEntityToLine = false;

                    // Save index of currently processed entity's index
                    uint currentEntityIndex = entityIndex;

                    // Fetch pointer to flow entity
                    std::shared_ptr<FlowEntity> spFlowEntity = mFlowEntities.at(currentEntityIndex);

                    // Check for new line. Is processed as all other entities. Will be always added to line
                    // because its width is zero
                    endOfParagraph = spFlowEntity->getType() == FlowEntity::Type::NewLine;

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
                        if (spFlowEntity->getType() == FlowEntity::Type::Word // is word
                            && entityIndex + 1 < mFlowEntities.size() // there is next entity
                            && mFlowEntities.at(entityIndex + 1)->getType() == FlowEntity::Type::Mark // next entity would be a mark
                            && newLineWidth + mFlowEntities.at(entityIndex + 1)->getPixelWidth() > mWidth // mark would not fit in same line
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
                        if (spFlowEntity->getType() == FlowEntity::Type::Space)
                        {
                            spaceFlowPartCount++;
                        }
                    }

                    // Check whether complete block of text is drawn or some flow part is left
                    if (flowPartIndex == flowPartCount)
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

                    // Add entity to line since at least some flow parts of it are drawn
                    if (addEntityToLine) { line.push_back(spFlowEntity); }

                    // Check whether there are entities left
                    hasNext = entityIndex < mFlowEntities.size();
				}

                // Remember longest line's width
                mFlowWidth = mFlowWidth < (int)glm::ceil(lineWidth) ? (int)glm::ceil(lineWidth) : mFlowWidth; // TODO: does this consider the dynamic spacing stuff for justify etc?

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

				// Decide alignment. Project natural alignment to either right or left
				TextFlowAlignment alignment = mAlignment;
				if (alignment == TextFlowAlignment::NATURAL)
				{
					if (globalRightToLeft)
					{
						alignment = TextFlowAlignment::RIGHT;
					}
					else
					{
						alignment = TextFlowAlignment::LEFT;
					}
				}

                // Decide dynamic space for line
				float dynamicSpace = mPixelOfSpace;
				if (
					alignment == TextFlowAlignment::JUSTIFY
					&& hasNext // do not use dynamic space for last line
					&& line.size() > 1) // do not use dynamic space when there is only one word
				{
                    dynamicSpace = ((float)mWidth - nonSpaceWidth) / drawnSpaceFlowPartCount;
				}

                // Calculate the new line width
                float newLineWidth = nonSpaceWidth + (drawnSpaceFlowPartCount * dynamicSpace);

				// Now decide xOffset for line (includes for right to left also change of starting position)
				float xOffset = 0;
				if (globalRightToLeft) // right to left direction
				{
					xOffset = newLineWidth; // left
					if (alignment == TextFlowAlignment::RIGHT) // right
					{
						xOffset = (float)mWidth;
					}
					else if(alignment == TextFlowAlignment::CENTER) // center
					{
						xOffset = (float)mWidth - (((float)mWidth - newLineWidth) / 2.f);
					}
				}
				else // left to right direction
				{
					xOffset = 0; // left
					if (alignment == TextFlowAlignment::RIGHT || alignment == TextFlowAlignment::CENTER) // right or center
					{
						xOffset = (float)mWidth - newLineWidth;
						if (alignment == TextFlowAlignment::CENTER) // center
						{
							xOffset /= 2.f;
						}
					}
				}

				// *** CLUSTER TO PARTS WITH HOMOGENE DIRECTION ***

				// TODO TESTING
				std::cout << "Start of part clustering" << std::endl;

				typedef std::pair< // pair of entity vector and direction indication
					std::vector< // vector of enties and pairs
						std::pair< // pair of entity and index
							std::shared_ptr<FlowEntity>, // shared pointer to entity
							uint>>, // index of entity within line
					bool> // right to left indication for this part
					Part;
				std::vector<Part> parts; // vector of parts in this line
				std::vector<std::pair<std::shared_ptr<FlowEntity>, uint> > helperPart; // only used in following loop
				bool rightToLeft = line.at(0)->isRightToLeft(); // assuming there is always at least one entity in a line
				for (uint entityIndex = 0; entityIndex < line.size(); entityIndex++)
				{
					std::shared_ptr<FlowEntity> spEntity = line.at(entityIndex);
					if (spEntity->isRightToLeft() != rightToLeft) // different direction for this entity
					{
						// Store so-far-collected entities including indication for direction
						parts.push_back(std::make_pair(helperPart, rightToLeft)); // deep copy of helper structure into part vector of this line
						helperPart.clear(); // clear helper part
						rightToLeft = spEntity->isRightToLeft(); // update current direction
					}

					// Add current entity into helper part, including entity index
					helperPart.push_back(std::make_pair(spEntity, entityIndex));
				}
				parts.push_back(std::make_pair(helperPart, rightToLeft)); // left over


				// *** DRAW LINE ***

                // Prepare xPixelPen for drawing
				float xPixelPen = xOffset;

				// Integrate entities' geometry to renderable mesh structure
                for(auto& rPart : parts) // go over directional homogene parts
				{
					// *** SIZE OF PART ***

					// Calculate horizontal size of this part
					float partWidth = 0;
					bool partRightToLeft = rPart.second; // direction of this part
					for (auto& rEntity : rPart.first) // go over entities of this part
					{
						// Extract entity and its index
						auto spEntity = rEntity.first; // actually, iterating over a pair. This is entity
						uint entityIndex = rEntity.second; // this is index of entity within line. Needed for following flowPartCount and index calculation

						// Go over flow parts
						uint flowPartCount = entityIndex == line.size() - 1 ? endFlowPartIndex + 1 : spEntity->getFlowPartCount();
						uint flowPartIndex = entityIndex == 0 ? initialFlowPartIndex : 0;
						for (; flowPartIndex < flowPartCount; flowPartIndex++) // go over flow parts count to draw of this entity
						{
							// Set width of flow parts that is not yet set
							if (spEntity->getType() == FlowEntity::Type::Space) // space
							{
								// Set pixel with of space flow part as fallback
								if (spEntity->mFlowParts.at(flowPartIndex)->isCollapsed())
								{
									spEntity->mFlowParts.at(flowPartIndex)->mPixelWidth = 0.f;
								}
								else
								{
									spEntity->mFlowParts.at(flowPartIndex)->mPixelWidth = dynamicSpace;
								}
							}

							// Accumulate width of this directional part
							partWidth += spEntity->mFlowParts.at(flowPartIndex)->getPixelWidth();
						}
					}

					// *** DRAWING OF PART ***

					// Create pixel pen used to draw the parts
					float partXPixelPen = xPixelPen; // standard when global and part have the same direction

					// For global direction of right to left, pen is at the right end of the canvas. So move it to the left end of the upcoming part to start the drawing from left to right
					if (globalRightToLeft && !partRightToLeft)
					{
						partXPixelPen -= partWidth;
					}
					else if (!globalRightToLeft && partRightToLeft) // symmetrical case
					{
						partXPixelPen += partWidth;
					}

					// Draw one part
					for (auto& rEntity : rPart.first) // go over enities in part
					{
						// Extract entity and its index
						auto spEntity = rEntity.first;
						uint entityIndex = rEntity.second;

						// Go over flow parts of current entity, which are in this line (same as above)
						uint flowPartCount = entityIndex == line.size() - 1 ? endFlowPartIndex + 1 : spEntity->getFlowPartCount();
						uint flowPartIndex = entityIndex == 0 ? initialFlowPartIndex : 0;
						for (; flowPartIndex < flowPartCount; flowPartIndex++) // go over flow parts to draw of this entity
						{
							// Advance partXPixelPen for right to left
							if (partRightToLeft)
							{
								// Width of flow part must be known at this time
								partXPixelPen -= spEntity->mFlowParts.at(flowPartIndex)->getPixelWidth();
							}

							// Fill position of complete entity
							if (pPreviousFilledEntity != line.at(entityIndex).get()) // fill it only once, so equal to first part's
							{
								spEntity->mX = (int)partXPixelPen;
								spEntity->mY = (int)(std::ceil(glm::abs(yPixelPen) - lineHeight));
							}

							// Fill position of entity part
							if (spEntity->getType() == FlowEntity::Type::Space) // space
							{
								// Save position in flow part
								spEntity->mFlowParts.at(flowPartIndex)->mX = (int)partXPixelPen;
								spEntity->mFlowParts.at(flowPartIndex)->mY = (int)(std::ceil(glm::abs(yPixelPen) - lineHeight));
							}
							else if (spEntity->getType() == FlowEntity::Type::NewLine) // new line
							{
								// Save position in flow part (different from other entities!)
								spEntity->mFlowParts.at(flowPartIndex)->mX = 0; // TODO: For right to left more like width?
								spEntity->mFlowParts.at(flowPartIndex)->mY = (int)(std::ceil(glm::abs(yPixelPen - lineHeight) - lineHeight));
							}
							else // word or mark
							{
								// Draw word of flow part
								if (spEntity->mFlowParts.at(flowPartIndex)->mupRenderWord != nullptr)
								{
									const auto* pRenderWord = line.at(entityIndex)->mFlowParts.at(flowPartIndex)->mupRenderWord.get();

									// Push back positions and texture coordinates
									for (uint i = 0; i < pRenderWord->spVertices->size(); i++)
									{
										const std::pair<glm::vec3, glm::vec2>& rVertex = pRenderWord->spVertices->at(i);
										rVertices.push_back(
											std::make_pair(
												glm::vec3(rVertex.first.x + partXPixelPen, rVertex.first.y + yPixelPen, rVertex.first.z),
												glm::vec2(glm::vec2(rVertex.second.s, rVertex.second.t))));
									}
								}
								else
								{
									throwWarning(OperationNotifier::Operation::BUG, "TextFlow has nullptr in flow part which should not exist");
									failure |= true;
								}

								// Save position in flow part
								spEntity->mFlowParts.at(flowPartIndex)->mX = (int)partXPixelPen;
								spEntity->mFlowParts.at(flowPartIndex)->mY = (int)(std::ceil(glm::abs(yPixelPen) - lineHeight));
							}

							// Advance partXPixelPen for left to right
							if (!partRightToLeft)
							{
								partXPixelPen += spEntity->mFlowParts.at(flowPartIndex)->getPixelWidth();
							}

						} // end of flow part iteration

						// Update xPixelPen
						if (globalRightToLeft)
						{
							xPixelPen -= partWidth;
						}
						else
						{
							xPixelPen += partWidth;
						}

						// Update previous filled entity
						pPreviousFilledEntity = spEntity.get();
					}  

                } // end of line

				// Advance yPen if there is a next line
				if (hasNext)
				{
					yPixelPen -= lineHeight;
				}

            } // end of lines
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
		}
    }

    std::vector<RenderWord> TextFlow::calculateFitWord(std::u16string content, int maxPixelWidth, float scale, bool rightToLeft) const
    {
        // Calculate word from content
        RenderWord word = calculateWord(content, scale, rightToLeft);

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
            std::vector<RenderWord> leftWord = calculateFitWord(content.substr(0, left), maxPixelWidth, scale, rightToLeft);
            std::vector<RenderWord> rightWord = calculateFitWord(content.substr(left, right), maxPixelWidth, scale, rightToLeft);

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

    bool TextFlow::insertFitWord(std::vector<RenderWord>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale, bool rightToLeft) const
    {
        // Do nothing if input is empty
        if (rContent.empty())
        {
            return true;
        }

        std::vector<RenderWord> newWords = calculateFitWord(rContent, maxPixelWidth, scale, rightToLeft);

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
			// TODO: some already defining in the character set
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
