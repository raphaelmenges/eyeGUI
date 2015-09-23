//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Sensor.h"

#include "Layout.h"
#include "OperationNotifier.h"
#include "Helper.h"

namespace eyegui
{
    Sensor::Sensor(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout* pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        float relativeScale,
        float border,
        std::string iconFilepath) : InteractiveElement(
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
        // Fill members
        mType = Type::SENSOR;
        mpRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::SENSOR,
            meshes::Type::QUAD);
        mPenetration = 0;
    }

    Sensor::~Sensor()
    {
        // Nothing to do here
    }

    void Sensor::penetrate(float amount)
    {
        mPenetration += amount;
        mPenetration = clamp(mPenetration, 0, 1);

        // Remove highlight
        highlight(false);
    }

    void Sensor::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        InteractiveElement::specialUpdate(tpf, pInput);

        // Penetration
        if (penetratedByInput(pInput))
        {
            // TODO: ugly input killing
            pInput->mouseUsed = true;

            mPenetration += tpf / mpLayout->getConfig()->sensorPenetrationIncreaseDuration;

            // Remove highlight
            highlight(false);
        }
        else
        {
            mPenetration -= tpf / mpLayout->getConfig()->sensorPenetrationDecreaseDuration;
        }
        mPenetration = clamp(mPenetration, 0, 1);

        // Inform listener after updating when penetrated
        if (mPenetration > 0)
        {
            mpLayout->enqueueNotification(this, Notification::SENSOR_PENETRATED);
        }
    }

    void Sensor::specialDraw() const
    {
        // Bind render item before setting values and drawing
        mpRenderItem->bind();

        // Super call
        InteractiveElement::specialDraw();

        // Fill other values
        mpRenderItem->getShader()->fillValue("penetration", mPenetration);

        // Scale of icon
        mpRenderItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());

        // Draw render item
        mpRenderItem->draw();
    }

    void Sensor::specialTransformAndSize()
    {
        // Nothing to do
    }

    void Sensor::specialReset()
    {
        InteractiveElement::specialReset();

        // Reset some values
        mPenetration = 0;
    }

    void Sensor::specialInteract()
    {
        penetrate(mpLayout->getConfig()->sensorInteractionPenetrationAmount);
    }

    void Sensor::specialPipeNotification(Notification notification)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case Notification::SENSOR_PENETRATED:
            notifyListener(&SensorListener::penetrated, mpLayout, getId(), mPenetration);
            break;
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Sensor got notification which is not thought for it.");
            break;
        }
    }
}
