//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Container.h"

namespace eyegui
{
	Container::Container(
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
		float innerBorder) : Block(
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
			innerBorder)
	{
		// Nothing to do
	}

	Container::~Container()
	{
		// Nothing to do
	}

	void Container::specialUpdate(float tpf, Input* pInput)
	{
		// Update the elements
		for (std::unique_ptr<Element>& element : mChildren)
		{
			element->update(tpf, mAlpha, pInput, mDimming.getValue());
		}

		// Super call after children (may consume input first)
		Block::specialUpdate(tpf, pInput);
	}

	void Container::specialDraw() const
	{
		Block::specialDraw();

		// Draw the elements
		for (const std::unique_ptr<Element>& element : mChildren)
		{
			element->draw();
		}
	}
}
