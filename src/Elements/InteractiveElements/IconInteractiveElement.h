//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Interactive element with single icon.

#ifndef ICON_INTERACTIVE_ELEMENT_H_
#define ICON_INTERACTIVE_ELEMENT_H_

#include "Elements/InteractiveElements/InteractiveElement.h"

namespace eyegui
{
	class IconInteractiveElement : public InteractiveElement
	{
	public:

		// Constructors
		IconInteractiveElement(
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
			std::string iconFilepath);

		// Destructor
		virtual ~IconInteractiveElement() = 0;

		// Set icon
		void setIcon(std::string filepath);

	protected:

		// Drawing filled by subclasses
		virtual void specialDraw() const;

		// Calculate aspect ratio correction for icon on gizmo
		glm::vec2 iconAspectRatioCorrection() const;

		// Members
		RenderItem const * mpIconRenderItem; // has to be initialized by subclasses

	private:

		// Members
		Texture const * mpIcon;
	};
}

#endif // ICON_INTERACTIVE_ELEMENT_H_