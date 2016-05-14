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
       // After construction, one can already construct the text.
       // Not like in text flow, which has to wait for a transform and resize
       calculateMesh();
    }

	TextSimple::TextSimple(const TextSimple& rOtherText) : Text(rOtherText)
	{
		// Copy members
		this->mWidth = rOtherText.mWidth;
		this->mHeight = rOtherText.mHeight;

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
		// Remind it at drawing
		mX = x;
		mY = y;
	}

    void TextSimple::draw(glm::vec4 color, float alpha, bool renderBackground) const
    {
		// TODO: render background if necessary

        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Calculate transformation matrix
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, glm::vec3(mX, mpGUI->getWindowHeight() - mY, 0)); // Change coordinate system and translate to position
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

    int TextSimple::getWidth() const
    {
        return mWidth;
    }

	int TextSimple::getHeight() const
	{
		return mHeight;
	}

	std::u16string TextSimple::getContent() const
	{
		return mContent;
	}

    void TextSimple::specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates)
    {
        // OpenGL setup done in calling method

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
        float yPixelPen = -lineHeight; // First line should be also inside element
        float maxPixelWidth = 0;
        float pixelHeight = 0;
        for(const std::u16string line : collectedLines)
        {
            // Just do whole line as one big word
            Word word = calculateWord(line, mScale);

            // Assuming, that the count of vertices and texture coordinates is equal
            for (uint i = 0; i < word.spVertices->size(); i++)
            {
                const glm::vec3& rVertex = word.spVertices->at(i);
                rVertices.push_back(glm::vec3(rVertex.x, rVertex.y + yPixelPen, rVertex.z));
                const glm::vec2& rTextureCoordinate = word.spTextureCoordinates->at(i);
                rTextureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
            }

            // Advance yPen
            yPixelPen -= lineHeight;

            // Remember that line for evaluate size
            maxPixelWidth = std::max(maxPixelWidth, word.pixelWidth);
            pixelHeight += lineHeight;
        }

        // Save used width and height
        mWidth = (int)maxPixelWidth;
        mHeight = (int)pixelHeight;
    }
}
