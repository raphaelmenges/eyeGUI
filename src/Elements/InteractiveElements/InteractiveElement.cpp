//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "InteractiveElement.h"

#include "Layout.h"
#include "Helper.h"

namespace eyegui
{
    InteractiveElement::InteractiveElement(
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
        bool adaptiveScaling) : NotifierElement(
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
        mType = Type::INTERACTIVE_ELEMENT;

        // Filling member
        mHighlight.setValue(0);
        mIsHighlighted = false;
        mSelection.setValue(0);
        mIsSelected = false;
    }

    InteractiveElement::~InteractiveElement()
    {
        // Nothing to do
    }

    bool InteractiveElement::isHighlighted() const
    {
        return mIsHighlighted;
    }

    void InteractiveElement::interact()
    {
        if (mAlpha >= 1 && isActive())
        {
            specialInteract();
        }
    }

    void InteractiveElement::highlight(bool doHighlight)
    {
        mIsHighlighted = doHighlight;
    }

    void InteractiveElement::select(bool doSelect)
    {
        mIsSelected = doSelect;
    }

    InteractiveElement* InteractiveElement::internalNextInteractiveElement(Element const * pCaller)
    {
        return this;
    }

    float InteractiveElement::specialUpdate(float tpf, Input* pInput)
    {
        // Highlight
        mHighlight.update(tpf / mpLayout->getConfig()->animationDuration, !mIsHighlighted);

        // Selection
        mSelection.update(tpf / mpLayout->getConfig()->animationDuration, !mIsSelected);

        // Instant interaction
        bool penetrated = penetratedByInput(pInput);
        if(penetrated && pInput->instantInteraction)
        {
            this->interact();
        }

        return 0;
    }

    void InteractiveElement::specialReset()
    {
        mHighlight.setValue(0);
        mIsHighlighted = false;
        mSelection.setValue(0);
        mIsSelected = false;
    }

    bool InteractiveElement::mayConsumeInput()
    {
        return true;
    }
}
