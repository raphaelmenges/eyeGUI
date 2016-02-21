//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Sensor.h"

#include "Layout.h"
#include "NotificationQueue.h"
#include "OperationNotifier.h"
#include "Helper.h"

namespace eyegui
{
    Sensor::Sensor(
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
        std::string iconFilepath) : IconInteractiveElement(
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
        // Fill members
        mType = Type::SENSOR;
        mpIconRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::SENSOR,
            meshes::Type::QUAD);
        mPenetration.setValue(0);
    }

    Sensor::~Sensor()
    {
        // Nothing to do here
    }

    void Sensor::penetrate(float amount)
    {
        // Penetrate
        mPenetration.update(amount);

        // Remove highlight
        highlight(false);
    }

    float Sensor::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        IconInteractiveElement::specialUpdate(tpf, pInput);

        // Penetration by input
        bool penetrated = penetratedByInput(pInput);
        if (penetrated)
        {
            // Remove highlight
            highlight(false);

            // Update of value
            mPenetration.update(tpf / mpLayout->getConfig()->sensorPenetrationIncreaseDuration);
        }
        else
        {
            // Update of value
            mPenetration.update(-tpf / mpLayout->getConfig()->sensorPenetrationDecreaseDuration);
        }

        // Inform listener after updating when penetrated
        if (mPenetration.getValue() > 0)
        {
            mpNotificationQueue->enqueue(getId(), NotificationType::SENSOR_PENETRATED);
        }

        return 0;
    }

    void Sensor::specialDraw() const
    {
        // Bind render item before setting values and drawing
		mpIconRenderItem->bind();

        // Super call
        IconInteractiveElement::specialDraw();

        // Fill other values
		mpIconRenderItem->getShader()->fillValue("penetration", mPenetration.getValue());

        // Scale of icon
		mpIconRenderItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());

        // Draw render item
		mpIconRenderItem->draw();
    }

	void Sensor::specialTransformAndSize()
	{
		// Nothing to do, but must be implemented
	}

    void Sensor::specialReset()
    {
		IconInteractiveElement::specialReset();

        // Reset some values
        mPenetration.setValue(0);
    }

    void Sensor::specialInteract()
    {
        penetrate(mpLayout->getConfig()->sensorInteractionPenetrationAmount);
    }

    void Sensor::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::SENSOR_PENETRATED:
            notifyListener(&SensorListener::penetrated, pLayout, getId(), mPenetration.getValue());
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Sensor got notification which is not thought for it.");
            break;
        }
    }
}
