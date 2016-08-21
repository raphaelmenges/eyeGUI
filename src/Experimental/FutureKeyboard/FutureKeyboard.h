//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_KEYBOARD_H_
#define FUTURE_KEYBOARD_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"

#include "src/Experimental/FutureKeyboard/FutureKey.h"

namespace eyegui
{
	class FutureKeyboard : public InteractiveElement
	{
	public:

		// Constructor
		FutureKeyboard(
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
		virtual ~FutureKeyboard();

	protected:

		// Updating filled by subclasses, returns adaptive scale
		virtual float specialUpdate(float tpf, Input* pInput);

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Transformation filled by subclasses
		virtual void specialTransformAndSize();

		// Reset filld by subclasses
		virtual void specialReset();

		// Interaction fill by subclasses
		virtual void specialInteract();

		// Filled by subclass and called by layout after updating and before drawing
		virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

	private:

		// Members
		RenderItem const * mpBackground;
		std::unique_ptr<FutureKey> mupFutureKey;
	};
}

#endif // FUTURE_KEYBOARD_H_
