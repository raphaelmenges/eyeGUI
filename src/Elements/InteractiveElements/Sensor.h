//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Sensor is penetrated by gaze or mouse hovering over the element. Listeners
// are notified when penetration is greater than zero.

#ifndef SENSOR_H_
#define SENSOR_H_

#include "InteractiveElement.h"
#include "Elements/NotifierTemplate.h"

namespace eyegui
{
	class Sensor : public InteractiveElement, public NotifierTemplate<SensorListener>
	{
	public:

		// Constructor
		Sensor(
			std::string id,
			std::string styleName,
			Element* pParent,
			Layout* pLayout,
			AssetManager* pAssetManager,
			float relativeScale,
			float border,
			std::string iconFilepath);

		// Deconstructor
		~Sensor();

		// Penetrating
		virtual void penetrate(float amount);

	protected:

		// Updating filled by subclasses
		virtual void specialUpdate(float tpf, Input* pInput);

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Transformation filled by subclasses
		virtual void specialTransformAndSize();

		// Reset filld by subclasses
		virtual void specialReset();

		// Interaction fill by subclasses
		virtual void specialInteract();

		// Filled by subclass and called by layout after updating and before drawing
		virtual void specialPipeNotification(Notification notification);

	private:

		// Members
		float mPenetration; // [0..1]
	};
}

#endif // CIRCLE_BUTTON_H_
