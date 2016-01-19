//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Key.h"

#include "Layout.h"
#include "AssetManager.h"

namespace eyegui
{
    Key::Key(Layout const * pLayout, AssetManager* pAssetManager)
    {
        // Initialize members
        mpLayout = pLayout;
        mpAssetManager = pAssetManager;

        // TransformAndSize has to be called before usage
        mX = 0;
        mY = 0;
        mSize = 0;

        // Fetch render item for key circle
        mpRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);
    }

    Key::~Key()
    {
        // Nothing to do
    }

    void Key::transformAndSize(int x, int y, int size)
    {
        mX = x;
        mY = y;
        mSize = size;

        mMatrix; // TODO
    }

    void Key::drawCircle(glm::vec4 color, float alpha) const
    {
        // Bind and fill render item
        mpRenderItem->bind();

        glm::vec4 circleColor = color;
        circleColor.a *= alpha;
        mpRenderItem->getShader()->fillValue("color", circleColor);

        // Transformation matrix
        glm::mat4 circleMatrix = Element::calculateDrawMatrix(
                mpLayout,
                mX - mSize/2,
                mY - mSize/2,
                mSize,
                mSize);
        mpRenderItem->getShader()->fillValue("matrix", circleMatrix);

        // Drawing
        mpRenderItem->draw();
    }

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
        mpShader = mpAssetManager->fetchShader(shaders::Type::CHARACTER_KEY);

        // Get glyph from font
        Glyph const * pGlyph = mpFont->getGlyph(FontSize::TALL, mCharacter);

        // Save currently set buffer and vertex array object
        GLint oldBuffer, oldVAO;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

        // Initialize mesh buffers and vertex array object
        glGenBuffers(1, &mVertexBuffer);
        glGenBuffers(1, &mTextureCoordinateBuffer);
        glGenVertexArrays(1, &mVertexArrayObject);

        // Fill vertex buffer (in OpenGL space)
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        std::vector<glm::vec3> vertices;
        vertices.push_back(glm::vec3(0,0,0));
        vertices.push_back(glm::vec3(1,0,0));
        vertices.push_back(glm::vec3(1,1,0));
        vertices.push_back(glm::vec3(1,1,0));
        vertices.push_back(glm::vec3(0,1,0));
        vertices.push_back(glm::vec3(0,0,0));
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * 3 * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        // Fill texture coordinate buffer
        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        std::vector<glm::vec2> textureCoordinates;
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.y));
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.z, pGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.w));
        textureCoordinates.push_back(glm::vec2(pGlyph->atlasPosition.x, pGlyph->atlasPosition.y));
        glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * 2 * sizeof(float), textureCoordinates.data(), GL_STATIC_DRAW);

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

    CharacterKey::~CharacterKey()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    void CharacterKey::draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const
    {
        // Draw circle of key
        drawCircle(color, alpha);

        // Render character
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);

        // Bind atlas texture
        glBindTexture(GL_TEXTURE_2D, mpFont->getAtlasTextureHandle(FontSize::TALL));

        // Color
        glm::vec4 characterColor = iconColor;
        iconColor.a *= alpha;

        // Fill uniforms
        mpShader->fillValue("matrix", mMatrix);
        mpShader->fillValue("color", characterColor);

        // Draw flow (vertex count must be 6)
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }

    std::u16string CharacterKey::getValue() const
    {
        return u"TODO";
    }
}
