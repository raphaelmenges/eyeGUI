//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "BoxButton.h"

namespace eyegui
{
    BoxButton::BoxButton(
        std::string id,
        std::vector<std::string> styles,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        std::string iconFilepath,
		std::u16string desc,
		std::string descKey,
        bool isSwitch,
		bool instantPress) : Button(
            id,
            styles,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            RenderingMask::BOX,
            false,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
			desc,
			descKey,
            isSwitch,
			instantPress)
    {
        mType = Type::BOX_BUTTON;

        // Render item
        mpBoxButtonItem = mpAssetManager->fetchRenderItem(
            shaders::Type::BOX_BUTTON,
            meshes::Type::QUAD);
    }

    BoxButton::~BoxButton()
    {
        // Nothing to do here
    }

    void BoxButton::specialDraw() const
    {
        mpIcon->bind(1);
        mpBoxButtonItem->bind();
        mpBoxButtonItem->getShader()->fillValue("matrix", mFullDrawMatrix);
        mpBoxButtonItem->getShader()->fillValue("color", getStyleValue(property::Color::Color));
        mpBoxButtonItem->getShader()->fillValue("iconColor", getStyleValue(property::Color::IconColor));
        mpBoxButtonItem->getShader()->fillValue("pressing", getPressing());
        mpBoxButtonItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());
        mpBoxButtonItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
        mpBoxButtonItem->getShader()->fillValue("mask", 0); // Mask is always in slot 0
        mpBoxButtonItem->getShader()->fillValue("icon", 1);
        mpBoxButtonItem->draw();

        // Super call (draw highlight etc. on top)
        Button::specialDraw();
    }
}
