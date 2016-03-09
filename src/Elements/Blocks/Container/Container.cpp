//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Container.h"

namespace eyegui
{
    Container::Container(
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
        bool adaptiveScaling,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        float innerBorder,
        bool showBackground) : Block(
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
            adaptiveScaling,
            consumeInput,
            backgroundFilepath,
            backgroundAlignment,
            innerBorder)
    {
        // Fill members
        mShowBackground = showBackground;
    }

    Container::~Container()
    {
        // Nothing to do
    }

    float Container::specialUpdate(float tpf, Input* pInput)
    {
        float maxAdaptiveScaleOfChildren = 0;

        // Update the elements
        for (std::unique_ptr<Element>& element : mChildren)
        {
            float childAdaptiveScale = element->update(tpf, mAlpha, pInput, mDim.getValue());
            maxAdaptiveScaleOfChildren = std::max(maxAdaptiveScaleOfChildren, childAdaptiveScale);
        }

        // Super call after children (may consume input first)
        float adaptiveScale = Block::specialUpdate(tpf, pInput);

        // Return adaptive scale
        return std::max(adaptiveScale, maxAdaptiveScaleOfChildren);
    }

    void Container::specialDraw() const
    {
        // Background is rendered by block superclass
        if(mShowBackground)
        {
            Block::specialDraw();
        }

        // Draw the elements
        for (const std::unique_ptr<Element>& element : mChildren)
        {
            element->draw();
        }
    }
}
