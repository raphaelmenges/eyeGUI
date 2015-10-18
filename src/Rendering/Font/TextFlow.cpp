//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextFlow.h"

#include "GUI.h"
#include "OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"
#include <cmath>

// TODO: Testing
#include <iostream>

namespace eyegui
{
    TextFlow::TextFlow(
        GUI const * pGUI,
        Font const * pFont,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        Shader const * pShader,
        std::u16string content)
    {
        // Fill members
        mpGUI = pGUI;
        mpFont = pFont;
        mFontSize = fontSize;
        mAlignment = alignment;
        mVerticalAlignment = verticalAlignment;
        mpShader = pShader;
        mContent = content;
        mFlowHeight = 0;

        // TransformAndSize have to done before usage
        mX = 0;
        mY = 0;
        mWidth = 0;
        mHeight = 0;

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

        // Get handle of atlas texture
		mAtlasTextureHandle = mpFont->getAtlasTextureHandle(mFontSize);

        // TransformAndSize HAS TO BE CALLED ONCE AT LAST!
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

    void TextFlow::draw(glm::vec4 color, float alpha) const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Calculate y offset because of vertical alignment
        int yOffset;
        switch(mVerticalAlignment)
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

        // If matrix were not calculated every frame, this would have to be notified about resizing...
        glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glm::vec3(mX, mpGUI->getWindowHeight() - (mY + yOffset), 0));
        matrix = glm::ortho(0.0f, (float)(mpGUI->getWindowWidth() - 1), 0.0f, (float)(mpGUI->getWindowHeight() - 1)) * matrix;

        // TODO: TEST (only one atlas at the moment)
        glBindTexture(GL_TEXTURE_2D, mAtlasTextureHandle);

        mpShader->fillValue("matrix", matrix);
        mpShader->fillValue("alpha", alpha);
        mpShader->fillValue("color", color);

        glDrawArrays(GL_TRIANGLES, 0, mVertexCount);
    }

    void TextFlow::calculateMesh()
    {
        // Save currently set buffer
        GLint oldBuffer;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);

        // Get size of space
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
			pixelOfSpace = pGlyph->advance.x;
		}

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

        // Go over paragraphs (pens are in local pixel coordinate system with origin in lower left corner)
        float yPixelPen = - mpFont->getLineHeight(mFontSize); // First line should be also inside flow
        for(std::u16string& rPargraph : paragraphs)
        {
            // Get words out of paragraph
            std::vector<TextFlow::Word> words;
            std::u16string wordDelimiter = u" ";
            while ((pos = rPargraph.find(wordDelimiter)) != std::u16string::npos)
            {
                token = rPargraph.substr(0, pos);
                words.push_back(calculateWord(token));
                rPargraph.erase(0, pos + wordDelimiter.length());
            }
            words.push_back(calculateWord(rPargraph)); // Last word (words never empty)
			
			// Check, whether all words can fit into given width
			float maxWordPixelWidth = 0;
			for (const Word& rWord : words)
			{
				maxWordPixelWidth = std::max(rWord.pixelWidth, maxWordPixelWidth);
			}

			// When all words could fit, try it
			if ((int)maxWordPixelWidth <= mWidth)
			{
				// Prepare some values
				int wordIndex = 0;
				bool hasNext = true;

				// Go over words in paragraph
				while (hasNext && (abs(yPixelPen) <= mHeight))
				{
					// Collect words in one line
					std::vector<Word const *> line;
					float wordsPixelWidth = 0;

					while (
						hasNext // Still words in the paragraph?
						&& ((wordsPixelWidth + words[wordIndex].pixelWidth) + (((int)line.size()) - 1) * pixelOfSpace) <= mWidth) // Still space in line?
					{
						wordsPixelWidth += words[wordIndex].pixelWidth;
						line.push_back(&words[wordIndex]);
						wordIndex++;

						if (wordIndex >= words.size())
						{
							// No words in paragraph left
							hasNext = false;
						}
					}

					// Now decide xOffset
					int xOffset = 0;
					if (mAlignment == TextFlowAlignment::RIGHT || mAlignment == TextFlowAlignment::CENTER)
					{
						xOffset = mWidth - int((wordsPixelWidth + ((int)line.size() - 1) * pixelOfSpace));
						if (mAlignment == TextFlowAlignment::CENTER)
						{
							xOffset = xOffset / 2;
						}
					}

					// Decide dynamic space
					int dynamicSpace = (int)pixelOfSpace;
					if (mAlignment == TextFlowAlignment::JUSTIFY && hasNext)
					{
						// Do not use dynamic space for last line
						dynamicSpace = (mWidth - (int)wordsPixelWidth) / ((int)line.size() - 1);
					}

					float xPixelPen = (float)xOffset;
					for (int i = 0; i < line.size(); i++)
					{
						// Assuming, that the count of vertices and texture coordinates is equal
						for (int j = 0; j < line[i]->spVertices->size(); j++)
						{
							glm::vec3& rVertex = line[i]->spVertices->at(j);
							vertices.push_back(glm::vec3(rVertex.x + xPixelPen, rVertex.y + yPixelPen, rVertex.z));
							glm::vec2& rTextureCoordinate = line[i]->spTextureCoordinates->at(j);
							textureCoordinates.push_back(glm::vec2(rTextureCoordinate.s, rTextureCoordinate.t));
						}

						// Advance xPen
						xPixelPen += (float)dynamicSpace + line[i]->pixelWidth;
					}

					// Advance yPen
					yPixelPen -= mpFont->getLineHeight(mFontSize);
				}
			}
        }

        // Get height of all lines (yPixelPen is one line to low now)
        mFlowHeight = (int)std::max(std::ceil(abs(yPixelPen) - mpFont->getLineHeight(mFontSize)), 0.0f);

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

    TextFlow::Word TextFlow::calculateWord(std::u16string content)
    {
        // Empty word
        Word word;
        word.spVertices = std::shared_ptr<std::vector<glm::vec3> >(new std::vector<glm::vec3>);
        word.spTextureCoordinates = std::shared_ptr<std::vector<glm::vec2> >(new std::vector<glm::vec2>);

        // Create data for text
        float xPixelPen = 0;
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

            float yPixelPen = 0 - (float)(pGlyph->size.y - pGlyph->bearing.y);

            // Vertices for this quad
            glm::vec3 vertexA = glm::vec3(xPixelPen, yPixelPen, 0);
            glm::vec3 vertexB = glm::vec3(xPixelPen + pGlyph->size.x, yPixelPen, 0);
            glm::vec3 vertexC = glm::vec3(xPixelPen + pGlyph->size.x, yPixelPen + pGlyph->size.y, 0);
            glm::vec3 vertexD = glm::vec3(xPixelPen, yPixelPen + pGlyph->size.y, 0);

            // Texture coordinates for this quad
            glm::vec2 textureCoordinateA = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateB = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateC = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w);
            glm::vec2 textureCoordinateD = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w);

			xPixelPen += pGlyph->advance.x;

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
}
