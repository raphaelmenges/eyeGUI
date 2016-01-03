//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Button in shape of a circle.

#ifndef CIRCLE_BUTTON_H_
#define CIRCLE_BUTTON_H_

#include "Button.h"

namespace eyegui
{
    class CircleButton : public Button
    {
    public:

        // Constructor
        CircleButton(
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
        ~CircleButton();

        // Check before transformation, how much space is needed
        virtual void evaluateSize(
            int availableWidth,
            int availableHeight,
            int& rWidth,
            int& rHeight) const;

    protected:

        // Drawing filled by subclasses
        virtual void specialDraw() const;

		// Checks, whether circle button is penetrated by input
		virtual bool penetratedByInput(Input const * pInput) const;
    };
}

#endif // CIRCLE_BUTTON_H_
