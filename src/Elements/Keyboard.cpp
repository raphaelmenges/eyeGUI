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

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Initialize keys
        newLine();

        // Add keys
        addKey(u"Q");
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

    void Keyboard::addKey(std::u16string content)
    {
        mKeys[mKeys.size()-1].push_back(
            std::unique_ptr<TextKey>(
                new TextKey(mpLayout, mpAssetManager, content)));
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

        // Fetch render item for key circle
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

    void Keyboard::Key::drawCircle(glm::vec4 color, float alpha) const
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

    Keyboard::TextKey::TextKey(
        Layout const * pLayout,
        AssetManager* pAssetManager,
        std::u16string content) : Key(pLayout, pAssetManager)
    {
        // Save members
        mContent = content;

        // Create text flow element
        mupTextFlow = pAssetManager->createTextFlow(FontSize::TALL, TextFlowAlignment::CENTER, TextFlowVerticalAlignment::CENTER, mContent);
    }

    void Keyboard::TextKey::transformAndSize(int x, int y, int size)
    {
        // Call super method
        Keyboard::Key::transformAndSize(x, y, size);

        // TODO: Set transformation of textflow
    }

    void Keyboard::TextKey::draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const
    {
        // Draw circle of key
        drawCircle(color, alpha);

        // Render text flow (TODO scale)
        mupTextFlow->draw(1, iconColor);
    }

    Keyboard::GraphicsKey::GraphicsKey(
        Layout const * pLayout,
        AssetManager* pAssetManager) : Key(pLayout, pAssetManager)
    {
        // TODO
    }
}
