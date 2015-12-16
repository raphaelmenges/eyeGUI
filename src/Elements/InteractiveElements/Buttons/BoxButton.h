//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Button in shape of a box.

#ifndef BOX_BUTTON_H_
#define BOX_BUTTON_H_

#include "Button.h"

namespace eyegui
{
    class BoxButton : public Button
    {
    public:

        // Constructor
        BoxButton(
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
            bool isSwitch);

        // Deconstructor
        ~BoxButton();

    protected:

        // Drawing filled by subclasses
        virtual void specialDraw() const;
    };
}

#endif // BOX_BUTTON_H_
