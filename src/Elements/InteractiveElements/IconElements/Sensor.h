//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Sensor is penetrated by gaze or mouse hovering over the element. Listeners
// are notified when penetration is greater than zero.

#ifndef SENSOR_H_
#define SENSOR_H_

#include "Elements/InteractiveElements/IconElements/IconElement.h"

namespace eyegui
{
    class Sensor : public IconElement, public NotifierTemplate<SensorListener>
    {
    public:

        // Constructor
        Sensor(
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
			std::string descKey);

        // Deconstructor
        ~Sensor();

        // Penetrating
        virtual void penetrate(float amount);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Reset filld by subclasses
        virtual void specialReset();

        // Interaction fill by subclasses
        virtual void specialInteract();

        // Filled by subclass and called by layout after updating and before drawing
        virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

    private:

        // Members
        LerpValue mPenetration; // [0..1]
        RenderItem const * mpSensorItem;
    };
}

#endif // CIRCLE_BUTTON_H_
