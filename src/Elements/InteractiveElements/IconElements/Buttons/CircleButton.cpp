//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "CircleButton.h"

namespace eyegui
{
    CircleButton::CircleButton(
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
        std::string iconFilepath,
		std::u16string desc,
		std::string descKey,
        bool isSwitch) : Button(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            RenderingMask::CIRCLE,
            true,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
			desc,
			descKey,
            isSwitch)
    {
        mType = Type::CIRCLE_BUTTON;

        // Fetch stuff for rendering
        mpCircleButtonItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_BUTTON, meshes::Type::QUAD);
    }

    CircleButton::~CircleButton()
    {
        // Nothing to do here
    }

    void CircleButton::specialDraw() const
    {
        mpIcon->bind(1);
        mpCircleButtonItem->bind();
        mpCircleButtonItem->getShader()->fillValue("matrix", mFullDrawMatrix);
        mpCircleButtonItem->getShader()->fillValue("color", getStyle()->color);
        mpCircleButtonItem->getShader()->fillValue("iconColor", getStyle()->iconColor);
        mpCircleButtonItem->getShader()->fillValue("pressing", getPressing());
        mpCircleButtonItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
        mpCircleButtonItem->getShader()->fillValue("mask", 0); // Mask is always in slot 0
        mpCircleButtonItem->getShader()->fillValue("icon", 1);
        mpCircleButtonItem->draw();


        // Super call
        Button::specialDraw();
    }

    void CircleButton::evaluateSize(
        int availableWidth,
        int availableHeight,
        int& rWidth,
        int& rHeight) const
    {
        // Keep button in square bound
        if (availableWidth > availableHeight)
        {
            rWidth = availableHeight;
            rHeight = availableHeight;
        }
        else
        {
            rWidth = availableWidth;
            rHeight = availableWidth;
        }
    }

    bool CircleButton::penetratedByInput(Input const * pInput) const
    {
        // Check whether gaze is upon circle button
        if (pInput != NULL && !pInput->gazeUsed)
        {
            // Length of vector from center to gaze point
            float radius = mWidth / 2.0f;
            float length = glm::length(
                            glm::vec2(
                                pInput->gazeX - (mX + radius),
                                pInput->gazeY - (mY + radius)));

            // Gaze inside circle?
            if(length <= radius)
            {
                return true;
            }
        }
        return false;
    }
}
