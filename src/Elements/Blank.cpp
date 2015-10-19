//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Blank.h"

namespace eyegui
{
    Blank::Blank(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout* pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
		NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border) : Element(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
			pNotificationQueue,
            relativeScale,
            border)
    {
        mType = Type::BLANK;
    }

    Blank::~Blank()
    {
        // Nothing to do
    }

    void Blank::specialUpdate(float tpf, Input* pInput)
    {
        // Nothing to do
    }

    void Blank::specialDraw() const
    {
        // Nothing to do
    }

    void Blank::specialTransformAndSize()
    {
        // Nothing to do
    }

    void Blank::specialReset()
    {
        // Nothing to do
    }
}
