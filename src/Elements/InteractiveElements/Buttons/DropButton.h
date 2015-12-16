//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Extending box button by taking a single child element. It is displayed when
// the button is down. That there is only one child element is guaranteed by the
// "attachInnerElement" method.

#ifndef DROP_BUTTON_H_
#define DROP_BUTTON_H_

#include "BoxButton.h"

namespace eyegui
{
    class DropButton : public BoxButton
    {
    public:

        // Constructor
        DropButton(
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
            float space);

        // Deconstructor
        ~DropButton();

        // Up and down
        virtual void down(bool immediately = false);
        virtual void up(bool immediately = false);

        // Attach inner element
        void attachInnerElement(std::unique_ptr<Element> upElement);

        // Replace an attached element, returns NULL if not found
        virtual std::unique_ptr<Element> replaceAttachedElement(
            Element* pTarget,
            std::unique_ptr<Element> upReplacement);

        // Starts the getting of the next interactive element, returns NULL if no available
        virtual InteractiveElement* nextInteractiveElement();

        // Activate or deactivate
        virtual void setActivity(bool active, bool fade);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Transformation
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

    private:

        // Getter for inner element
        Element* getInnerElement();

        // Member
        float mSpace;
        LerpValue mInnerAlpha;
        bool mInnerElementVisible;
    };
}

#endif // DROP_BUTTON_H_
