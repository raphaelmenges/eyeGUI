//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "CharacterKey.h"

#include "src/Layout.h"
#include "src/Rendering/AssetManager.h"
#include "src/Defines.h"
#include "src/Utilities/Helper.h"

namespace eyegui
{
    CharacterKey::CharacterKey(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        Font const * pFont,
        char16_t character) : Key(pLayout, pAssetManager)
    {
        // Save members
        mpFont = pFont;
        mCharacter = character;

        // Fetch shader to render character
        mpQuadShader = mpAssetManager->fetchShader(shaders::Type::CHARACTER_KEY);

        // Get glyph from font
        mpGlyph = mpFont->getGlyph(FontSize::KEYBOARD, mCharacter);

        // Calculate relative size of character
        float targetGlyphHeight = mpFont->getTargetGlyphHeight(FontSize::KEYBOARD);
        mCharacterSize = std::max(KEY_MIN_SCALE, mpGlyph->size.y / targetGlyphHeight);

        // Prepare quad for displaying the character
        prepareQuad();
    }

    CharacterKey::CharacterKey(const CharacterKey& rOtherKey) : Key(rOtherKey)
    {
        // Copy members
        mpFont = rOtherKey.mpFont;
        mCharacter = rOtherKey.mCharacter;
        mpQuadShader = rOtherKey.mpQuadShader;
        mpGlyph = rOtherKey.mpGlyph;
        mCharacterSize = rOtherKey.mCharacterSize;

        // But create own quad!
        prepareQuad();
    }

    CharacterKey::~CharacterKey()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mQuadVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mQuadVertexBuffer);
        glDeleteBuffers(1, &mQuadTextureCoordinateBuffer);
    }

    void CharacterKey::transformAndSize()
    {
        // Super call
        Key::transformAndSize();

        // Calculate ratio of glyph representing character
        float ratio = (float)mpGlyph->size.x / (float)mpGlyph->size.y;

        // Calculate size multiplier
        glm::vec2 sizeMultiplier;
        if (ratio > 1)
        {
            // Wider
            sizeMultiplier = glm::vec2(1, 1.0f / ratio);
        }
        else
        {
            // Higher
            sizeMultiplier = glm::vec2(ratio, 1);
        }

        // Fill matrix for rendering quad displaying character
        glm::vec2 quadSize = sizeMultiplier * (float)mSize * KEY_CIRCLE_CHARACTER_SIZE_RATIO;
        mQuadMatrix = calculateDrawMatrix(
            mpLayout->getLayoutWidth(),
            mpLayout->getLayoutHeight(),
            mX - (int)(quadSize.x / 2),
            mY - (int)(quadSize.y / 2),
            (int)quadSize.x,
            (int)quadSize.y);

        // Set texture coordinates of quad
        GLint oldBuffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadTextureCoordinateBuffer);
        std::vector<glm::vec2> textureCoordinates;
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.z, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(mpGlyph->atlasPosition.x, mpGlyph->atlasPosition.y));
        glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * 2 * sizeof(float), textureCoordinates.data(), GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }

    void CharacterKey::draw(
            glm::vec4 color,
            glm::vec4 pickColor,
            glm::vec4 iconColor,
			glm::vec4 thresholdColor,
			float threshold,
            float alpha) const
    {
        // Draw circle of key
        drawCircle(
            color,
            pickColor,
            alpha);

        // Render character
        mpQuadShader->bind();
        glBindVertexArray(mQuadVertexArrayObject);

        // Bind atlas texture
        mpFont->bindAtlasTexture(FontSize::KEYBOARD, 1, true);

        // Fill color
        glm::vec4 characterColor = iconColor;
        characterColor.a *= alpha;
        mpQuadShader->fillValue("color", characterColor);

        // Fill other uniforms
        mpQuadShader->fillValue("matrix", mQuadMatrix); // Matrix is updated in transform and size
        mpQuadShader->fillValue("atlas", 1);

        // Draw character quad (vertex count must be 6)
        glDrawArrays(GL_TRIANGLES, 0, 6);

		// Draw threshold
		drawThreshold(
			thresholdColor,
			threshold,
			alpha);
    }

    std::u16string CharacterKey::getValue() const
    {
        return std::u16string(&mCharacter, 1);
    }

    void CharacterKey::prepareQuad()
    {
        // Save currently set buffer and vertex array object
        GLint oldBuffer = -1;
        GLint oldVAO = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

        // Initialize mesh buffers and vertex array object
        glGenBuffers(1, &mQuadVertexBuffer);
        glGenBuffers(1, &mQuadTextureCoordinateBuffer);
        glGenVertexArrays(1, &mQuadVertexArrayObject);

        // Coordinates of quad
        float border = (1.0f - mCharacterSize) / 2.0f;
        float a = border;
        float b = 1.0f - border;

        // Fill vertex buffer (in OpenGL space)
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(a, a, 0));
        vertices.push_back(glm::vec3(b, a, 0));
        vertices.push_back(glm::vec3(b, b, 0));
        vertices.push_back(glm::vec3(b, b, 0));
        vertices.push_back(glm::vec3(a, b, 0));
        vertices.push_back(glm::vec3(a, a, 0));
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Texture coordinates are dynamic and filled in transformAndSize method

        // Bind stuff to vertex array object
        glBindVertexArray(mQuadVertexArrayObject);

        // Vertices
        GLuint vertexAttrib = glGetAttribLocation(mpQuadShader->getShaderProgram(), "posAttribute");
        glEnableVertexAttribArray(vertexAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadVertexBuffer);
        glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Texture coordinates
        GLuint uvAttrib = glGetAttribLocation(mpQuadShader->getShaderProgram(), "uvAttribute");
        glEnableVertexAttribArray(uvAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mQuadTextureCoordinateBuffer);
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Restore old settings
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
        glBindVertexArray(oldVAO);
    }
}
