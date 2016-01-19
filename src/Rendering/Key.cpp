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
        mX = 100;
        mY = 100;
        mSize = 100;

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
    }

    void Key::drawCircle(glm::vec4 color, float alpha) const
    {
        // Bind and fill render item
        mpRenderItem->bind();
        mpRenderItem->getShader()->fillValue("color", color);
        mpRenderItem->getShader()->fillValue("alpha", alpha);

        // Transformation matrix
        glm::mat4 matrix = Element::calculateDrawMatrix(
                mpLayout,
                mX - mSize/2,
                mY - mSize/2,
                mSize,
                mSize);
        mpRenderItem->getShader()->fillValue("matrix", matrix);

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

        // Generate character quad

    }

    CharacterKey::~CharacterKey()
    {
        // Delete vertex array object
        glDeleteVertexArrays(1, &mVertexArrayObject);

        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    void CharacterKey::transformAndSize(int x, int y, int size)
    {
        // Call super method
        Key::transformAndSize(x, y, size);

        // TODO: Set transformation of textflow
    }

    void CharacterKey::draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const
    {
        // Draw circle of key
        drawCircle(color, alpha);

        // Render character
    }

    std::u16string CharacterKey::getValue() const
    {
        return u"TODO";
    }
}
