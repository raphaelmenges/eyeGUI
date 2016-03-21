//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Sensor.h"

#include "Layout.h"
#include "NotificationQueue.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"

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
        std::string iconFilepath,
		std::u16string desc,
		std::string descKey) : IconElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            RenderingMask::BOX,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            iconFilepath,
			desc,
			descKey)
    {
        // Fill members
        mType = Type::SENSOR;

        // Initialize members
        mPenetration.setValue(0);

        // Render item
        mpSensorItem = mpAssetManager->fetchRenderItem(
            shaders::Type::SENSOR,
            meshes::Type::QUAD);

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
		float adaptiveScale = IconElement::specialUpdate(tpf, pInput);

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

        return adaptiveScale;
    }

    void Sensor::specialDraw() const
    {
        mpIcon->bind(1);
        mpSensorItem->bind();
        mpSensorItem->getShader()->fillValue("matrix", mFullDrawMatrix);
        mpSensorItem->getShader()->fillValue("color", getStyle()->color);
        mpSensorItem->getShader()->fillValue("iconColor", getStyle()->iconColor);
        mpSensorItem->getShader()->fillValue("penetration", mPenetration.getValue());
        mpSensorItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());
        mpSensorItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
        mpSensorItem->getShader()->fillValue("mask", 0); // Mask is always in slot 0
        mpSensorItem->getShader()->fillValue("icon", 1);
        mpSensorItem->draw();

        // Super call
        IconElement::specialDraw();
    }

    void Sensor::specialReset()
    {
        IconElement::specialReset();

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
