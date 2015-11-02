//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Element.h"

#include "Layout.h"
#include "Helper.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

#include <algorithm>

namespace eyegui
{
	Element::Element(
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
		bool adaptiveScaling)
	{
		// Initialize members
		mType = Type::ELEMENT;
		mId = id;
		mStyleName = styleName;
		mpParent = pParent;
		mpLayout = pLayout;
		mpFrame = pFrame;
		mpAssetManager = pAssetManager;
		mpNotificationQueue = pNotificationQueue;
		mX, mY, mWidth, mHeight = 0;
		mRelativeScale = relativeScale;
		mBorder = border;
		mDimmable = dimmable;
		mActive = true;
		mActivity.setValue(1);
		mDimming.setValue(0);
		mForceUndim = false;
		mAlpha = 1;
		mBorderAspectRatio = 1;
		mAdaptiveScaling = adaptiveScaling;
		mAdaptiveScale.setValue(0);

		// Fetch style from layout
		mpStyle = mpLayout->getStyleFromStylesheet(mStyleName);
	}

	Element::~Element()
	{
		// Nothing to do so far
	}

	Element::Type Element::getType() const
	{
		return mType;
	}

	std::string Element::getId() const
	{
		return mId;
	}

	Element* Element::getParent() const
	{
		return mpParent;
	}

	bool Element::isActive() const
	{
		return mActive;
	}

	Element::Orientation Element::getOrientation() const
	{
		return mOrientation;
	}

	Style const * Element::getStyle() const
	{
		return mpStyle;
	}

	std::string Element::getStyleName() const
	{
		return mStyleName;
	}

	void Element::setAlpha(float alpha)
	{
		mAlpha = alpha;
	}

	float Element::getAlpha() const
	{
		return mAlpha;
	}

	void Element::setActivity(bool active, bool fade)
	{
		if (active == mActive)
		{
			// Nothing to do
			return;
		}
		else
		{
			if (active)
			{
				if (mpParent->isActive())
				{
					mActive = true;

					// Do it immediatelly, maybe
					if (!fade)
					{
						mActivity.setValue(1);
					}

					// Do it for all children
					for (const std::unique_ptr<Element>& element : mChildren)
					{
						element.get()->setActivity(true, fade);
					}
				}
			}
			else
			{
				mActive = false;

				// Do it immediately, maybe
				if (!fade)
				{
					mActivity.setValue(0);
				}

				// Do it for all children
				for (const std::unique_ptr<Element>& element : mChildren)
				{
					element.get()->setActivity(false, fade);
				}
			}
		}
	}

	void Element::setDimmable(bool dimmable)
	{
		mDimmable = dimmable;
	}

	bool Element::isDimmable() const
	{
		return mDimmable;
	}

	Layout const * Element::getLayout() const
	{
		return mpLayout;
	}

	Frame* Element::getFrame() const
	{
		return mpFrame;
	}

	AssetManager* Element::getAssetManager() const
	{
		return mpAssetManager;
	}

	NotificationQueue* Element::getNotificationQueue() const
	{
		return mpNotificationQueue;
	}

	float Element::getBorder() const
	{
		return mBorder;
	}

	std::set<Element*> Element::getAllChildren() const
	{
		std::set<Element*> elements;

		// Go over children and collect pointers
		for (const std::unique_ptr<Element>& rupChild : mChildren)
		{
			std::set<Element*> childrensElements = rupChild->getAllChildren();
			elements.insert(childrensElements.begin(), childrensElements.end());
			elements.insert(rupChild.get());
		}

		return elements;
	}

	std::set<std::string> Element::getAllChildrensIds() const
	{
		std::set<std::string> ids;

		// Go over children and collect ids
		for (const std::unique_ptr<Element>& rupChild : mChildren)
		{
			// Insert id of children of child
			std::set<std::string> childrensIds = rupChild->getAllChildrensIds();
			ids.insert(childrensIds.begin(), childrensIds.end());

			// Only insert id of child if there is one
			std::string id = rupChild->getId();
			if (id != "")
			{
				ids.insert(rupChild->getId());
			}
		}

		return ids;
	}

	void Element::transformAndSize(int x, int y, int width, int height)
	{
		// Use border
		int usedBorder;
		if (width > height)
		{
			usedBorder = (int)((float)height * mBorder);
		}
		else
		{
			usedBorder = (int)((float)width * mBorder);
		}

		// Keep aspect ratio in mind (for example: keep aspect ratio of picture)
		if (mBorderAspectRatio > 1)
		{
			// Wider
			x += (int)((usedBorder / 2) * mBorderAspectRatio);
			width -= (int)((usedBorder)* mBorderAspectRatio);

			y += usedBorder / 2;
			height -= usedBorder;
		}
		else
		{
			// Higher
			x += usedBorder / 2;
			width -= usedBorder;

			y += (int)((usedBorder / 2) * mBorderAspectRatio);
			height -= (int)(usedBorder * mBorderAspectRatio);
		}

		// Save values to members
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;

		// Set orientation
		if (mWidth > mHeight)
		{
			mOrientation = Element::Orientation::HORIZONTAL;
		}
		else
		{
			mOrientation = Element::Orientation::VERTICAL;
		}

		// Call method implemented by subclasses
		specialTransformAndSize();

		// After calculation transformation, recalculate the matrix for rendering
		mDrawMatrix = calculateDrawMatrix(mX, mY, mWidth, mHeight);
	}


	int Element::getX() const
	{
		return mX;
	}
	int Element::getY() const
	{
		return mY;
	}

	int Element::getWidth() const
	{
		return mWidth;
	}

	int Element::getHeight() const
	{
		return mHeight;
	}

	float Element::getDynamicScale() const
	{
		return mRelativeScale + (mAdaptiveScale.getValue() * mpLayout->getConfig()->maximalAdaptiveScaleIncrease);
	}

	float Element::getRelativeScale() const
	{
		return mRelativeScale;
	}

	bool Element::getAdaptiveScaling() const
	{
		return mAdaptiveScaling;
	}

	float Element::getRelativePositionOnLayoutX() const
	{
		return (float)mX / mpLayout->getLayoutWidth();
	}

	float Element::getRelativePositionOnLayoutY() const
	{
		return (float)mY / mpLayout->getLayoutHeight();
	}

	float Element::getRelativeSizeOnLayoutX() const
	{
		return (float)mWidth / mpLayout->getLayoutWidth();
	}

	float Element::getRelativeSizeOnLayoutY() const
	{
		return (float)mHeight / mpLayout->getLayoutHeight();
	}

	float Element::update(float tpf, float alpha, Input* pInput, float dimming)
	{
		// Activity animationa
		mActivity.update(tpf, !mActive);

		// Save current alpha (already animated by layout or other element)
		mAlpha = alpha;

		// Use activity and alpha to check whether input is necessary
		if (mAlpha < 1 || mActivity.getValue() < 1)
		{
			pInput = NULL;
		}

		// Simple test whether mouse is over element (TODO: only mouse at the moment)
		bool penetrated = penetratedByInput(pInput);

		// Dimming
		if (mForceUndim)
		{
			// Undim it
			mDimming.update(-tpf / mpLayout->getConfig()->dimmingDecreaseDuration);
		}
		else if (mDimmable)
		{
			// TODO: do something more intelligent for input testing
			if (penetrated)
			{
				// Undim it
				mDimming.update(-tpf / mpLayout->getConfig()->dimmingDecreaseDuration);
			}
			else
			{
				// Dim it
				mDimming.update(tpf / mpLayout->getConfig()->dimmingIncreaseDuration);
			}
		}
		else
		{
			// Use dimming value of parent
			mDimming.setValue(dimming);
		}

		// Adaptive scaling
		if (mAdaptiveScaling)
		{
			// Set maximum of adaptive scale
			if (penetrated)
			{
				// Scale it up
				mAdaptiveScale.update(tpf / mpLayout->getConfig()->adaptiveScaleIncreaseDuration);
			}
			else
			{
				// Scale it down
				mAdaptiveScale.update(-tpf / mpLayout->getConfig()->adaptiveScaleDecreaseDuration);
			}
		}
		else
		{
			// Scale it down
			mAdaptiveScale.update(-tpf / mpLayout->getConfig()->adaptiveScaleDecreaseDuration);
		}

		// Update replaced element if there is some
		if (mupReplacedElement.get() != NULL)
		{
			float replacedAlpha = mAlpha * (mupReplacedElement->getAlpha()
				- (tpf / mpLayout->getConfig()->animationDuration));
			replacedAlpha = clamp(replacedAlpha, 0, 1);
			mupReplacedElement->update(tpf, replacedAlpha, NULL, mDimming.getValue());

			// Check, whether replacement is still visible
			if (replacedAlpha <= 0)
			{
				// Give it to layout for destruction at end of frame
				mpFrame->commitDyingReplacedElement(std::move(mupReplacedElement));
			}
		}

		// Call specialized update of subclasses
		float specialAdaptiveScale = specialUpdate(tpf, pInput);

		// Return adaptive scale. Either own or calculated in special update with children
		return std::max(mAdaptiveScale.getValue(), specialAdaptiveScale);
	}

	void Element::draw() const
	{
		// Only draw if visible
		if (mAlpha > 0)
		{
			// Draw the element
			specialDraw();

			// Draw fading replaced elements if available (always mutliplied with own alpha)
			if (mupReplacedElement.get() != NULL)
			{
				mupReplacedElement->draw();
			}
		}
	}

	void Element::reset()
	{
		mActive = true;
		mActivity.setValue(1);

		mDimming.setValue(0);
		mForceUndim = false;

		mAdaptiveScale.setValue(0);

		// Do reset implemented by subclass
		specialReset();

		// Go over chilren and reset
		for (std::unique_ptr<Element>& element : mChildren)
		{
			element->reset();
		}
	}

	void Element::evaluateSize(
		int availableWidth,
		int availableHeight,
		int& rWidth,
		int& rHeight) const
	{
		rWidth = availableWidth;
		rHeight = availableHeight;
	}

	InteractiveElement* Element::nextInteractiveElement()
	{
		if (getParent() != NULL)
		{
			// Ok, go one up and look. Only interactive elements should be called.
			return getParent()->internalNextInteractiveElement(this);
		}
		else
		{
			// Top layer, call yourself
			return internalNextInteractiveElement(NULL);
		}
	}

	InteractiveElement* Element::internalNextInteractiveElement(Element const * pChildCaller)
	{
		// This is no interactive element ...
		if (getParent() != NULL)
		{
			// ... so go one up to parent a hope the best.
			return getParent()->internalNextInteractiveElement(this);
		}
		else
		{
			// ... so it is root and no interactive element can be selected
			return NULL;
		}
	}

	std::unique_ptr<Element> Element::replaceAttachedElement(Element* pTarget,
		std::unique_ptr<Element> upReplacement)
	{
		// Search in children for element to replace
		int index = -1;
		int i = 0;
		for (std::unique_ptr<Element>& rupElement : mChildren)
		{
			if (rupElement.get() == pTarget)
			{
				index = i;
				break;
			}
			i++;
		}

		// Child found, so replace it
		if (index > -1)
		{
			// Replace it
			std::unique_ptr<Element> upTarget = std::move(mChildren[i]);
			mChildren[i] = std::move(upReplacement);
			return std::move(upTarget);
		}

		// Fallback if not found
		return NULL;
	}

	void Element::commitReplacedElement(std::unique_ptr<Element> upElement, bool fade)
	{
		if (fade)
		{
			// Fading is wished, so remember the replaced element
			mupReplacedElement = std::move(upElement);
		}
		else
		{
			// No fading, so let it die instantly after this frame
			mpFrame->commitDyingReplacedElement(std::move(upElement));
		}
	}

	glm::mat4 Element::calculateDrawMatrix(int x, int y, int width, int height) const
	{
		// Get values from layout
		float layoutWidth = (float)(mpLayout->getLayoutWidth());
		float layoutHeight = (float)(mpLayout->getLayoutHeight());

		// Create identity
		glm::mat4 matrix = glm::mat4(1.0f);

		// Width and height from zero to one
		float glWidth = width / layoutWidth;
		float glHeight = height / layoutHeight;

		// Moving
		matrix = glm::translate(
			matrix,
			glm::vec3(x / layoutWidth,
				(1.0f - (y / layoutHeight)) - glHeight,
				0));

		// Scaling
		matrix = glm::scale(matrix, glm::vec3(glWidth, glHeight, 1));

		// Projection
		matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f) * matrix;

		return matrix;
	}

	bool Element::penetratedByInput(Input const * pInput) const
	{
		// Standard check, testing whether cursor is above button
		if (pInput != NULL && !pInput->mouseUsed)
		{
			if (pInput->mouseCursorX >= mX
				&& pInput->mouseCursorX <= mX + mWidth
				&& pInput->mouseCursorY >= mY
				&& pInput->mouseCursorY <= mY + mHeight)
			{
				return true;
			}
		}
		return false;
	}
}
