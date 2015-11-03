//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Interactive element is an extened element that supports highlighting,
// interaction, selection and notifications.

#ifndef INTERACTIVE_ELEMENT_H_
#define INTERACTIVE_ELEMENT_H_

#include "Elements/Element.h"

namespace eyegui
{
	class InteractiveElement : public Element
	{
	public:

		// Ugly enumeration, but somehow layout must remember to call correct noficiaton
		enum class Notification { BUTTON_HIT, BUTTON_DOWN, BUTTON_UP, SENSOR_PENETRATED };

		// Constructors
		InteractiveElement(
			std::string id,
			std::string styleName,
			Element* pParent,
			Layout const * pLayout,
			Frame* pFrame,
			AssetManager* pAssetManager,
			NotificationQueue* pNotificationQueue,
			float relativeScale,
			float border,
			bool dimmable,
			bool adaptiveScaling,
			std::string iconFilepath);

		// Destructor
		virtual ~InteractiveElement() = 0;

		// Check, whether highlighted
		bool isHighlighted() const;

		// Interaction
		void interact();

		// Highlight button, returns whether successful
		void highlight(bool doHighlight);

		// Select button, returns whether successful
		void select(bool doSelect);

		// Set icon
		void setIcon(std::string filepath);

		// Called by layout after updating
		void pipeNotification(Notification notification, Layout* pLayout);

		// Tries to fetch next interactive element for selecting, returns NULL if fails
		virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

	protected:

		// Updating filled by subclasses, returns adaptive scale
		virtual float specialUpdate(float tpf, Input* pInput);

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Reset filld by subclasses
		virtual void specialReset();

		// Interaction fill by subclasses
		virtual void specialInteract() = 0;

		// Filled by subclass and called by layout after updating
		virtual void specialPipeNotification(Notification notification, Layout* pLayout) = 0;

		// Calculate aspect ratio correction for icon on gizmo
		glm::vec2 iconAspectRatioCorrection() const;

		// Members
		RenderItem const * mpRenderItem; // has to be initialized by subclasses

	private:

		// Members
		LerpValue mHighlight;
		bool mIsHighlighted;
		LerpValue mSelection;
		bool mIsSelected;
		Texture const * mpIcon;
	};
}

#endif // INTERACTIVE_ELEMENT_H_
