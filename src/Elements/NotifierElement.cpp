//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "NotifierElement.h"

namespace eyegui
{
    NotifierElement::NotifierElement(
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
        bool adaptiveScaling) : Element(
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
        mType = Type::NOTIFIER_ELEMENT;
    }

    NotifierElement::~NotifierElement()
    {
        // Nothing to do
    }

    void NotifierElement::pipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Only pipe if visible (so fading elements do not notify)
        if (mAlpha >= 1)
        {
            specialPipeNotification(notification, pLayout);
        }
    }
}
