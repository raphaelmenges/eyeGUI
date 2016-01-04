//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Element.h"

namespace eyegui
{
	class Keyboard : public Element
	{
	public:

		// Constructor
		Keyboard(
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
			bool adaptiveScaling);

		// Destructor
		virtual ~Keyboard();

	protected:

		// Updating filled by subclasses, returns adaptive scale
		virtual float specialUpdate(float tpf, Input* pInput);

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Transformation filled by subclasses
		virtual void specialTransformAndSize();

		// Reset filled by subclasses
		virtual void specialReset();

		// Implemented by subclasses
		virtual bool mayConsumeInput();

		// Members
		RenderItem const * mpBackground;
	};
}

#endif // KEYBOARD_H_
