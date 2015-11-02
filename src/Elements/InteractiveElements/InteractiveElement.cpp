//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "InteractiveElement.h"

#include "Layout.h"
#include "Helper.h"

namespace eyegui
{
	InteractiveElement::InteractiveElement(
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
		std::string iconFilepath) : Element(
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
			adaptiveScaling)
	{
		mType = Type::INTERACTIVE_ELEMENT;

		// Filling members
		setIcon(iconFilepath);
		mpRenderItem = NULL;
		mHighlight.setValue(0);
		mIsHighlighted = false;
		mSelection.setValue(0);
		mIsSelected = false;
	}

	InteractiveElement::~InteractiveElement()
	{
		// Nothing to do
	}

	bool InteractiveElement::isHighlighted() const
	{
		return mIsHighlighted;
	}

	void InteractiveElement::interact()
	{
		if (mAlpha >= 1 && isActive())
		{
			specialInteract();
		}
	}

	void InteractiveElement::highlight(bool doHighlight)
	{
		mIsHighlighted = doHighlight;
	}

	void InteractiveElement::select(bool doSelect)
	{
		mIsSelected = doSelect;
	}

	void InteractiveElement::setIcon(std::string filepath)
	{
		if (filepath != EMPTY_STRING_ATTRIBUTE)
		{
			mpIcon = mpAssetManager->fetchTexture(filepath);
		}
		else
		{
			mpIcon = mpAssetManager->fetchTexture(graphics::Type::NOT_FOUND);
		}
	}

	void InteractiveElement::pipeNotification(Notification notification, Layout* pLayout)
	{
		// Only pipe if visible (so fading elements do not notify)
		if (mAlpha >= 1)
		{
			specialPipeNotification(notification, pLayout);
		}
	}

	InteractiveElement* InteractiveElement::internalNextInteractiveElement(Element const * pCaller)
	{
		return this;
	}

	float InteractiveElement::specialUpdate(float tpf, Input* pInput)
	{
		// Highlight
		mHighlight.update(tpf, !mIsHighlighted);

		// Selection
		mSelection.update(tpf / mpLayout->getConfig()->animationDuration, !mIsSelected);

		return 0;
	}

	void InteractiveElement::specialDraw() const
	{
		// Fill shader
		mpRenderItem->getShader()->fillValue("matrix", mDrawMatrix);
		mpRenderItem->getShader()->fillValue("highlight", mHighlight.getValue());
		mpRenderItem->getShader()->fillValue("alpha", mAlpha);
		mpRenderItem->getShader()->fillValue("activity", mActivity.getValue());
		mpRenderItem->getShader()->fillValue("selection", mSelection.getValue());
		mpRenderItem->getShader()->fillValue("color", getStyle()->color);
		mpRenderItem->getShader()->fillValue("highlightColor", getStyle()->highlightColor);
		mpRenderItem->getShader()->fillValue("selectionColor", getStyle()->selectionColor);
		mpRenderItem->getShader()->fillValue("iconColor", getStyle()->iconColor);
		mpRenderItem->getShader()->fillValue("time", mpLayout->getAccPeriodicTime());
		mpRenderItem->getShader()->fillValue("dimColor", getStyle()->dimColor);
		mpRenderItem->getShader()->fillValue("dimming", mDimming.getValue());

		// Bind icon texture
		mpIcon->bind(0);
	}

	void InteractiveElement::specialReset()
	{
		mHighlight.setValue(0);
		mIsHighlighted = false;
		mSelection.setValue(0);
		mIsSelected = false;
	}

	glm::vec2 InteractiveElement::iconAspectRatioCorrection() const
	{
		float aspectRatio = (float)mWidth / (float)mHeight;
		float iconAspectRatio = mpIcon->getAspectRatio();
		float relation = aspectRatio / iconAspectRatio;
		glm::vec2 iconUVScale;
		if (relation >= 1)
		{
			// Render item wider than icon
			iconUVScale = glm::vec2(relation, 1.0f);
		}
		else
		{
			// Icon wider than render item
			iconUVScale = glm::vec2(1.0f, 1.0f / relation);
		}

		return iconUVScale;
	}
}
