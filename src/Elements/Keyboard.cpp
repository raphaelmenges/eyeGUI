//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"


// TODO: Testing
#include <iostream>

namespace eyegui
{
    Keyboard::Keyboard(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling) : Element(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::KEYBOARD;

        // Save members
        mpFont = mpAssetManager->getKeyboardFont();

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Initialize keys
        newLine();

        // Add keys
        addKey(u'Q');
        /*addKey(u'W');
        addKey(u'E');
        addKey(u'R');
        addKey(u'T');
        addKey(u'Y');
        newLine();
        addKey(u'A');
        addKey(u'S');
        addKey(u'D');
        addKey(u'F');
        addKey(u'G'); */
    }

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // TODO: update keys (maybe do not save keycount but vector with matrices and...vector with letters...)
        return 0;
    }

    void Keyboard::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("backgroundColor", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", mAlpha);
            mpBackground->getShader()->fillValue("activity", mActivity.getValue());
            mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpBackground->getShader()->fillValue("dim", mDim.getValue());
            mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
            mpBackground->getShader()->fillValue("mark", mMark.getValue());
            mpBackground->draw();
        }

        // *** RENDER KEYS ***
        for(const auto& rLine : mKeys)
        {
            for(const auto& rKey : rLine)
            {
                rKey->draw(getStyle()->color, getStyle()->iconColor, mAlpha);
            }
        }
    }

    void Keyboard::specialTransformAndSize()
    {
        // TODO
    }

    void Keyboard::specialReset()
    {
        // Nothing to do
    }

    bool Keyboard::mayConsumeInput()
    {
        return true;
    }

    void Keyboard::addKey(char16_t character)
    {
        mKeys[mKeys.size()-1].push_back(
            std::unique_ptr<FontKey>(
                new FontKey(mpLayout, mpAssetManager, mpFont, character)));
    }

    void Keyboard::newLine()
    {
        mKeys.push_back(std::vector<std::unique_ptr<Key> >());
    }

    Keyboard::Key::Key(Layout const * pLayout, AssetManager* pAssetManager)
    {
        // Initialize members
        mpLayout = pLayout;
        mX = 100;
        mY = 100;
        mSize = 100;

        // Fetch render item for keys
        mpRenderItem = pAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);
    }

    void Keyboard::Key::transformAndSize(int x, int y, int size)
    {
        mX = x;
        mY = y;
        mSize = size;
    }

    Keyboard::FontKey::FontKey(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        Font const * pFont,
        char16_t character) : Key(pLayout, pAssetManager)
    {
        mCharacter = character;
        mpFont = pFont;
    }

    void Keyboard::FontKey::draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const
    {
        // Bind and fill render item
        mpRenderItem->bind();
        mpRenderItem->getShader()->fillValue("color", color);
        mpRenderItem->getShader()->fillValue("iconColor", iconColor);
        mpRenderItem->getShader()->fillValue("alpha", alpha);

        // Transformation matrix
        glm::mat4 matrix = Element::calculateDrawMatrix(
                mpLayout,
                mX - mSize/2,
                mY - mSize/2,
                mSize,
                mSize);
        mpRenderItem->getShader()->fillValue("matrix", matrix);

        // Atlas
        glBindTexture(GL_TEXTURE_2D, mpFont->getAtlasTextureHandle(FontSize::TALL));

        // Position on atlas
        Glyph const * pGlyph = mpFont->getGlyph(FontSize::TALL, mCharacter);
        mpRenderItem->getShader()->fillValue("atlasPosition", pGlyph->atlasPosition);

        // Drawing
        mpRenderItem->draw();
    }

    Keyboard::GraphicsKey::GraphicsKey(
        Layout const * pLayout,
        AssetManager* pAssetManager) : Key(pLayout, pAssetManager)
    {
        // TODO
    }
}
