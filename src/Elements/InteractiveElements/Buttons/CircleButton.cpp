//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "CircleButton.h"

namespace eyegui
{
	CircleButton::CircleButton(
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
		mType = Type::CIRCLE_BUTTON;

		// Fetch stuff for rendering
		mpRenderItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_BUTTON, meshes::Type::QUAD);
	}

	CircleButton::~CircleButton()
	{
		// Nothing to do here
	}

	void CircleButton::specialDraw() const
	{
		// Bind render item before setting values and drawing
		mpRenderItem->bind();

		// Super call
		Button::specialDraw();

		// Draw render item
		mpRenderItem->draw();
	}

	void CircleButton::evaluateSize(
		int availableWidth,
		int availableHeight,
		int& rWidth,
		int& rHeight) const
	{
		// Keep button in square bound
		if (availableWidth > availableHeight)
		{
			rWidth = availableHeight;
			rHeight = availableHeight;
		}
		else
		{
			rWidth = availableWidth;
			rHeight = availableWidth;
		}
	}
}
