//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "BoxButton.h"

namespace eyegui
{
	BoxButton::BoxButton(
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
		std::string iconFilepath,
		bool isSwitch) : Button(
			id,
			styleName,
			pParent,
			pLayout,
			pFrame,
			pAssetManager,
			pNotificationQueue,
			relativeScale,
			border,
			dimmable,
			adaptiveScaling,
			iconFilepath,
			isSwitch)
	{
		mType = Type::BOX_BUTTON;

		// Fetch stuff for rendering
		mpRenderItem = mpAssetManager->fetchRenderItem(
			shaders::Type::BOX_BUTTON,
			meshes::Type::QUAD);
	}

	BoxButton::~BoxButton()
	{
		// Nothing to do here
	}

	void BoxButton::specialDraw() const
	{
		// Bind render item before setting values and drawing
		mpRenderItem->bind();

		// Super call
		Button::specialDraw();

		// Scale of icon
		mpRenderItem->getShader()->fillValue("iconUVScale", iconAspectRatioCorrection());

		// Orientation
		float orientation = 0;
		if (getParent() != NULL)
		{
			if (getParent()->getOrientation() == Element::Orientation::VERTICAL)
			{
				orientation = 1;
			}
		}
		mpRenderItem->getShader()->fillValue("orientation", orientation);

		// Draw render item
		mpRenderItem->draw();
	}
}
