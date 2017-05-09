//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextSimple.h"

#include "src/GUI.h"
#include "src/Rendering/AssetManager.h"
#include "src/Utilities/OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

namespace eyegui
{
    TextSimple::TextSimple(
        GUI const * pGUI,
        AssetManager* pAssetManager,
        Font const * pFont,
        FontSize fontSize,
        float scale,
        std::u16string content) : Text(
            pGUI,
            pAssetManager,
            pFont,
            fontSize,
            scale,
            content)
    {
       mTextWidth = 0;
       mTextHeight = 0;

       // After construction, one can already construct the text.
       // Not like in text flow, which has to wait for a transform and resize
       calculateMesh();
    }

	TextSimple::TextSimple(const TextSimple& rOtherText) : Text(rOtherText)
	{
		// Copy members
        this->mTextWidth = rOtherText.mTextWidth;
        this->mTextHeight = rOtherText.mTextHeight;

		// But create own mesh stuff!
		calculateMesh();
	}

    TextSimple::~TextSimple()
    {
        // Nothing to do
    }

    void TextSimple::transform()
    {
        // Calculate mesh since window size could have been changed
        calculateMesh();
    }

	void TextSimple::setPosition(int x, int y)
	{
        // Remember it at drawing
		mX = x;
		mY = y;
	}

	void TextSimple::draw(
		glm::vec4 color,
		float alpha,
		bool renderBackground,
		int xOffset,
		int yOffset) const
    {
        // Draw background
        if (renderBackground)
        {
            drawSimpleBackground(
                mTextWidth,
                mTextHeight,
                mTextWidth,
                mTextHeight,
                0,
                xOffset,
                yOffset,
                alpha);
        }

        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Calculate transformation matrix
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(
			matrix,
			glm::vec3(
				mX + xOffset,
				mpGUI->getWindowHeight() - mY + yOffset,
				0)); // Change coordinate system and translate to position
        matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix; // Pixel to world space

        // Bind atlas texture
        if (mScale == 1.0f)
        {
            mpFont->bindAtlasTexture(mFontSize, 1, false);
        }
        else
        {
            mpFont->bindAtlasTexture(mFontSize, 1, true);
        }

        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("color", color);
        mpShader->fillValue("alpha", alpha);
        mpShader->fillValue("atlas", 1);

        // Draw flow
        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

	int TextSimple::getX() const
	{
		return mX;
	}

	int TextSimple::getY() const
	{
		return mY;
	}

    int TextSimple::getTextWidth() const
    {
        return mTextWidth;
    }

    int TextSimple::getTextHeight() const
	{
        return mTextHeight;
	}

	std::u16string TextSimple::getContent() const
	{
		return mContent;
	}

    void TextSimple::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight,
            RenderWordVertices& rVertices)
    {
        // OpenGL setup done in calling method

		// Determine text direction by iterating through the content until first indicating character
		bool globalRightToLeft = false;
		for (const auto& character : streamlinedContent)
		{
			auto direction = mpFont->getCharacterDirection(character);
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

        // Go over lines and collect them
        std::u16string delimiter = u"\n";
        size_t pos = 0;
        std::vector<std::u16string> collectedLines;
        while ((pos = streamlinedContent.find(delimiter)) != std::u16string::npos)
        {
            collectedLines.push_back(streamlinedContent.substr(0, pos));
            streamlinedContent.erase(0, pos + delimiter.length());
        }
        collectedLines.push_back(streamlinedContent);

        // Go over collected lines
        float yPixelPen = -lineHeight; // first line should be also inside element
        float maxPixelWidth = 0;
        float pixelHeight = 0;
		std::vector<
			std::pair<
				std::vector<std::pair<glm::vec3, glm::vec2> >, // position and texture coordinates
				float> > // length of the line (used for right hand alignment if necessary)
			vertices; // vertices per line
        for(const std::u16string line : collectedLines)
        {
			// Collect parts of that line (some may left to right, other right to left in terms of direction)
			std::vector<RenderWord> parts;

			// Go over characters
			bool directionUndecided = true;
			bool partRightToLeft = false;
			int startIndex = 0;
			int characterCount = line.length();
			for (int index = 0; index < characterCount; index++)
			{
				// Determine direction
				CharacterDirection direction = mpFont->getCharacterDirection(line.at(index));
				if (directionUndecided) // direction not yet decided
				{
					if (direction == CharacterDirection::LeftToRight)
					{
						partRightToLeft = false;
						directionUndecided = false;
					}
					else if (direction == CharacterDirection::RightToLeft)
					{
						partRightToLeft = true;
						directionUndecided = false;
					}
				}
				else // direction has been initially decided for this line
				{
					if (direction == CharacterDirection::LeftToRight)
					{
						if (!partRightToLeft) { continue; } // direction continues, ok
						else
						{
							parts.push_back(calculateWord(line.substr(startIndex, index - startIndex), mScale, true)); // previous part
							startIndex = index;
							partRightToLeft = false;
						}
					}
					else if (direction == CharacterDirection::RightToLeft)
					{
						if (partRightToLeft) { continue; } // direction continues, ok
						else
						{
							parts.push_back(calculateWord(line.substr(startIndex, index - startIndex), mScale, false)); // previous part
							startIndex = index;
							partRightToLeft = true;
						}
					}			
				}
			}

			// Push back remaining part
			if (startIndex < characterCount)
			{
				parts.push_back(calculateWord(line.substr(startIndex, characterCount - startIndex ), mScale, partRightToLeft));
			}

            // Process collected parts
			std::vector<std::pair<glm::vec3, glm::vec2> > lineVertices; // vertices of this line
			float xPixelPen = 0.f;
			int partCount = (int)parts.size();
			for (int i = 0; i < partCount; i++)
			{
				// If right to left text direction, switch index direction to render last part at first
				int index = i;
				if (globalRightToLeft)
				{
					index = partCount - index - 1;
				}

				// Push back vertices for this line
				for (uint j = 0; j < parts.at(index).spVertices->size(); j++)
				{
					const std::pair<glm::vec3, glm::vec2>& rVertex = parts.at(index).spVertices->at(j);
					lineVertices.push_back(
						std::make_pair(
							glm::vec3(rVertex.first.x + xPixelPen, rVertex.first.y + yPixelPen, rVertex.first.z),
							glm::vec2(glm::vec2(rVertex.second.s, rVertex.second.t))));
				}

				// Expand xPixelPen
				xPixelPen += parts.at(index).pixelWidth;
			}

            // Advance yPen
            yPixelPen -= lineHeight;

            // Remember that line for evaluate size
            maxPixelWidth = std::max(maxPixelWidth, xPixelPen);
            pixelHeight += lineHeight;

			// Push back vertices to collection
			vertices.push_back(std::make_pair(lineVertices, xPixelPen)); // store vertices and width for this line
        }

		// Concatenation of the vertices
		for (auto& line : vertices)
		{
			// Change alignment to right hand side if required
			if (globalRightToLeft)
			{
				// Add offset
				float offset = maxPixelWidth - line.second;
				for (auto & vertex : line.first)
				{
					vertex.first.x += offset;
				}
			}

			// Insert vertices to given reference
			rVertices.insert(rVertices.end(), line.first.begin(), line.first.end());
		}

        // Save used width and height
        mTextWidth = (int)maxPixelWidth;
        mTextHeight = (int)pixelHeight;
    }
}
