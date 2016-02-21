//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Button.h"

#include "Layout.h"
#include "NotificationQueue.h"
#include "OperationNotifier.h"
#include "Helper.h"

namespace eyegui
{
    Button::Button(
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
        bool isSwitch) : IconInteractiveElement(
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
            adaptiveScaling,
            iconFilepath)
    {
        mType = Type::BUTTON;

        // Fill members
        mIsSwitch = isSwitch;

        // Calling virtual reset method in constructor is not good
        mIsDown = false;
        mThreshold.setValue(0);
        mPressing.setValue(0);
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
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_HIT);

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
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_DOWN);

            // Immediately
            if (immediately)
            {
                mPressing.setValue(1);
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
            mpNotificationQueue->enqueue(getId(), NotificationType::BUTTON_UP);

            // Immediately
            if (immediately)
            {
                mPressing.setValue(0);
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

    float Button::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = IconInteractiveElement::specialUpdate(tpf, pInput);

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Pressing animation
        if (mIsDown && mPressing.getValue() < 1)
        {
            mPressing.update(tpf / mpLayout->getConfig()->buttonPressingDuration);
        }
        else if (!mIsDown && mPressing.getValue() > 0)
        {
            mPressing.update(-tpf / mpLayout->getConfig()->buttonPressingDuration);
        }

        // If pressed and no switch, go back (do it each frame and not only at end of going down,
        // because otherwise a deactivation would prohibit going up because of the test for
        // activity in up()
        if (mIsDown && !mIsSwitch && mPressing.getValue() >= 1)
        {
            up();
        }

        // Threshold
        if (
            (mPressing.getValue() == 0 || mPressing.getValue() == 1) // Only when completey up or down
            && penetrated // Penetration
            && !(!mIsSwitch && mPressing.getValue() > 0)) // Avoids to add threshold for none switch when at down position
        {
            mThreshold.update(tpf / mpLayout->getConfig()->buttonThresholdIncreaseDuration);

            if (mThreshold.getValue() >= 1)
            {
                hit();
                mThreshold.setValue(0);
            }
        }
        else
        {
            mThreshold.update(-tpf / mpLayout->getConfig()->buttonThresholdDecreaseDuration);
        }

        return adaptiveScale;
    }

    void Button::specialDraw() const
    {
        // Super call
        IconInteractiveElement::specialDraw();

		mpIconRenderItem->getShader()->fillValue("threshold", mThreshold.getValue());
		mpIconRenderItem->getShader()->fillValue("pressing", mPressing.getValue());
    }

	void Button::specialTransformAndSize()
	{
		// Nothing to do, but must be implemented
	}

    void Button::specialReset()
    {
		IconInteractiveElement::specialReset();

        mIsDown = false;
        mThreshold.setValue(0);
        mPressing.setValue(0);
    }

    void Button::specialInteract()
    {
        // Hit button
        hit();

        // Reset threshold
        mThreshold.setValue(0);
    }

    void Button::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::BUTTON_HIT:
            notifyListener(&ButtonListener::hit, pLayout, getId());
            break;
        case NotificationType::BUTTON_DOWN:
            notifyListener(&ButtonListener::down, pLayout, getId());
            break;
        case NotificationType::BUTTON_UP:
            notifyListener(&ButtonListener::up, pLayout, getId());
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Button got notification which is not thought for it.");
            break;
        }
    }
}
