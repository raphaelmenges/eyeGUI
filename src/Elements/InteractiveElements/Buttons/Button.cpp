//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Button.h"

#include "Layout.h"
#include "OperationNotifier.h"
#include "Helper.h"

namespace eyegui
{
    Button::Button(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout* pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        float relativeScale,
        float border,
        std::string iconFilepath,
        bool isSwitch) : InteractiveElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            relativeScale,
            border,
            iconFilepath)
    {
        mType = Type::BUTTON;

        // Fill members
        mIsSwitch = isSwitch;

        // Calling virtual reset method in constructor is not good
        mIsDown = false;
        mThreshold = 0;
        mPressing = 0;
    }

    Button::~Button()
    {
        // Nothing to do so far
    }

    void Button::hit(bool immediately)
    {
        if (mActive)
        {
            // Inform listener after updating
            mpLayout->enqueueNotification(this, Notification::BUTTON_HIT);

            // Call context correct method
            if (mIsDown)
            {
                up(immediately);
            }
            else
            {
                down(immediately);
            }
        }
    }

    void Button::down(bool immediately)
    {
        if (!mIsDown && mActive)
        {
            // Remove highlight
            highlight(false);

            // Save state
            mIsDown = true;

            // Inform listener after updating
            mpLayout->enqueueNotification(this, Notification::BUTTON_DOWN);

            // Immediately
            if (immediately)
            {
                mPressing = 1;
            }
        }
    }

    void Button::up(bool immediately)
    {
        if (mIsDown && mActive)
        {
            // Remove highlight
            highlight(false);

            // Save state
            mIsDown = false;

            // Inform listener after updating
            mpLayout->enqueueNotification(this, Notification::BUTTON_UP);

            // Immediately
            if (immediately)
            {
                mPressing = 0;
            }
        }
    }

    bool Button::isSwitch() const
    {
        return mIsSwitch;
    }

    bool Button::isDown() const
    {
        return mIsDown;
    }

    void Button::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        InteractiveElement::specialUpdate(tpf, pInput);

        // TODO: more abstract (not only mouse) -> Settings, to say on which input it should react?

        bool penetrated = penetratedByInput(pInput);
        if (penetrated)
        {
            // TODO: Just special testcase for mouse input
            pInput->mouseUsed = true;
        }

        // Pressing animation
        if (mIsDown && mPressing < 1)
        {
            mPressing += tpf / mpLayout->getConfig()->buttonPressingDuration;

            // If pressed and no switch, go back
            if (!mIsSwitch && mPressing >= 1)
            {
                up();
            }
        }
        else if (!mIsDown && mPressing > 0)
        {
            mPressing -= tpf / mpLayout->getConfig()->buttonPressingDuration;
        }
        mPressing = clamp(mPressing, 0, 1);

        // Threshold
        if ((mPressing == 0 || mPressing == 1) && penetrated)
        {
            mThreshold += tpf / mpLayout->getConfig()->buttonThresholdIncreaseDuration;

            if (mThreshold >= 1)
            {
                hit();
                mThreshold = 0;
            }
        }
        else
        {
            mThreshold -= tpf / mpLayout->getConfig()->buttonThresholdDecreaseDuration;
        }
        mThreshold = clamp(mThreshold, 0, 1);
    }

    void Button::specialDraw() const
    {
        // Super call
        InteractiveElement::specialDraw();

        mpRenderItem->getShader()->fillValue("threshold", mThreshold);
        mpRenderItem->getShader()->fillValue("pressing", mPressing);
    }

    void Button::specialTransformAndSize()
    {
        // Do nothing
    }

    void Button::specialReset()
    {
        InteractiveElement::specialReset();

        mIsDown = false;
        mThreshold = 0;
        mPressing = 0;
    }

    void Button::specialInteract()
    {
        hit();
    }

    void Button::specialPipeNotification(Notification notification)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case Notification::BUTTON_HIT:
            notifyListener(&ButtonListener::hit, mpLayout, getId());
            break;
        case Notification::BUTTON_DOWN:
            notifyListener(&ButtonListener::down, mpLayout, getId());
            break;
        case Notification::BUTTON_UP:
            notifyListener(&ButtonListener::up, mpLayout, getId());
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Button got notification which is not thought for it.");
            break;
        }
    }
}
