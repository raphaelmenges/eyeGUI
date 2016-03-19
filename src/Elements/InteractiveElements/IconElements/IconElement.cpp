//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "IconElement.h"

#include "Layout.h"

namespace eyegui
{
    IconElement::IconElement(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        RenderingMask renderingMask,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        std::string iconFilepath) : InteractiveElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            renderingMask,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::ICON_ELEMENT;

        // Filling members
        setIcon(iconFilepath);
    }

    IconElement::~IconElement()
    {
        // Nothing to do here
    }

    void IconElement::setIcon(std::string filepath)
    {
        if (filepath != EMPTY_STRING_ATTRIBUTE)
        {
            mpIcon = mpAssetManager->fetchTexture(filepath);
        }
        else
        {
            mpIcon = mpAssetManager->fetchTexture(graphics::Type::NOT_FOUND);
        }
    }

    void IconElement::specialDraw() const
    {
        // Draw stuff like highlighting
        InteractiveElement::specialDraw();
    }

    glm::vec2 IconElement::iconAspectRatioCorrection() const
    {
        float aspectRatio = (float)mWidth / (float)mHeight;
        float iconAspectRatio = mpIcon->getAspectRatio();
        float relation = aspectRatio / iconAspectRatio;
        glm::vec2 iconUVScale;
        if (relation >= 1)
        {
            // Render item wider than icon
            iconUVScale = glm::vec2(relation, 1.0f);
        }
        else
        {
            // Icon wider than render item
            iconUVScale = glm::vec2(1.0f, 1.0f / relation);
        }

        return iconUVScale;
    }
}
