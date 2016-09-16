//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Text.h"

#include "src/GUI.h"
#include "src/Rendering/AssetManager.h"
#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{

    Text::Text(
        GUI const * pGUI,
        AssetManager* pAssetManager,
        Font const * pFont,
        FontSize fontSize,
        float scale,
        std::u16string content)
    {
        // Fill members
        mpGUI = pGUI;
        mpAssetManager = pAssetManager;
        mpFont = pFont;
        mFontSize = fontSize;
        mScale = scale;
        mContent = content;

        // Fetch shader
        mpShader = mpAssetManager->fetchShader(shaders::Type::TEXT);

        // Initialize some members (x and y are somehow set by specialized classes)
        mX = 0;
        mY = 0;
        mVertexCount = 0;

		// Background
		mpBackground = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);

		// Prepare OpenGL stuff for text
		prepareText();
	}

	Text::Text(const Text& rOtherText)
	{
		this->mpGUI = rOtherText.mpGUI;
		this->mpAssetManager = rOtherText.mpAssetManager;
		this->mpFont = rOtherText.mpFont;
		this->mFontSize = rOtherText.mFontSize;
		this->mScale = rOtherText.mScale;
		this->mX = rOtherText.mX;
		this->mY = rOtherText.mY;
		this->mContent = rOtherText.mContent;
		this->mpShader = rOtherText.mpShader;
		this->mVertexCount = 0;

		// Create own text for the copy
		prepareText();
	}

    Text::~Text()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    void Text::setContent(std::u16string content)
    {
        mContent = content;
        calculateMesh();
    }

    void Text::calculateMesh()
    {
        // Save currently set buffer
        GLint oldBuffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);

        // Get height of line
        float lineHeight = mScale * mpFont->getLineHeight(mFontSize);

        // Go over words in content
        std::u16string copyContent = mContent;

        // Streamline line ending
        streamlineLineEnding(copyContent);

        // Structures for OpenGL
        RenderWordVertices vertices;

        // Let the subclasses to the work
        specialCalculateMesh(copyContent, lineHeight, vertices);

        // Vertex count
        mVertexCount = (GLuint)vertices.size();

        // Create buffers with positions and texture coordinates
        std::vector<glm::vec3> positions;
        positions.reserve(mVertexCount);
        std::vector<glm::vec2> textureCoordinates;
        textureCoordinates.reserve(mVertexCount);
        for(int i = 0; i < mVertexCount; i++)
        {
            positions.push_back(vertices.at(i).first);
            textureCoordinates.push_back(vertices.at(i).second);
        }

        // Fill into buffer
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 3 * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, mVertexCount * 2 * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);

        // Restore old setting
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }

    RenderWord Text::calculateWord(const std::u16string& rContent, float scale) const
    {
        // Empty word
        RenderWord word;
        word.spVertices = std::shared_ptr<RenderWordVertices>(new RenderWordVertices);
        word.lettersXOffsets.reserve(rContent.size());

        // Fill word with data
        float xPixelPen = 0;
        for (uint i = 0; i < rContent.size(); i++)
        {
            Glyph const * pGlyph = mpFont->getGlyph(mFontSize, rContent.at(i));
            if (pGlyph == NULL)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "TextFlow has character in content not covered by character set");
                continue;
            }

            float yPixelPen = 0 - (scale * (float)(pGlyph->size.y - pGlyph->bearing.y));

            // Positions for this quad
            glm::vec3 positionA = glm::vec3(xPixelPen, yPixelPen, 0);
            glm::vec3 positionB = glm::vec3(xPixelPen + (scale * pGlyph->size.x), yPixelPen, 0);
            glm::vec3 positionC = glm::vec3(xPixelPen + (scale * pGlyph->size.x), yPixelPen + (scale * pGlyph->size.y), 0);
            glm::vec3 positionD = glm::vec3(xPixelPen, yPixelPen + (scale * pGlyph->size.y), 0);

            // Texture coordinates for this quad
            glm::vec2 textureCoordinateA = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateB = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y);
            glm::vec2 textureCoordinateC = glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w);
            glm::vec2 textureCoordinateD = glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w);

			// Advance x pen
            xPixelPen += scale * (float)pGlyph->advance.x;

			// Push back x offset of letters
			word.lettersXOffsets.push_back(xPixelPen);

            // Fill into data blocks
            word.spVertices->push_back(std::make_pair(positionA, textureCoordinateA));
            word.spVertices->push_back(std::make_pair(positionB, textureCoordinateB));
            word.spVertices->push_back(std::make_pair(positionC, textureCoordinateC));
            word.spVertices->push_back(std::make_pair(positionC, textureCoordinateC));
            word.spVertices->push_back(std::make_pair(positionD, textureCoordinateD));
            word.spVertices->push_back(std::make_pair(positionA, textureCoordinateA));
        }

        // Set width of whole word
        word.pixelWidth = xPixelPen;

        return word;
    }

    RenderWord Text::calculateWord(const char16_t& rLetter, float scale) const
    {
        // Create string out of char
        std::u16string string(&rLetter, 1);

        // Delegate to standard
        return std::move(calculateWord(string, scale));
    }

	void Text::prepareText()
	{
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
}
