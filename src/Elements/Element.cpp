//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Element.h"

#include "Layout.h"
#include "Helper.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"
#include "OperationNotifier.h"

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
        bool dimming,
        bool adaptiveScaling) : Object()
    {
        // Initialize members
        mX, mY, mWidth, mHeight = 0;
        mOrientation = Element::Orientation::VERTICAL;
        mType = Type::ELEMENT;
        mId = id;
        mStyleName = styleName;
        mpParent = pParent;
        mpLayout = pLayout;
        mpFrame = pFrame;
        mpAssetManager = pAssetManager;
        mpNotificationQueue = pNotificationQueue;
        mRelativeScale = relativeScale;
        mBorder = border;
        mDimming = dimming;
        mActive = true;
        mActivity.setValue(1);
        mDim.setValue(0);
        mForceUndim = false;
        mAlpha = 1;
        mBorderAspectRatio = 1;
        mAdaptiveScaling = adaptiveScaling;
        mAdaptiveScale.setValue(0);
        mHidden = false;
        mMarking = false;
        mMark.setValue(0);

        // Fetch style from layout
        mpStyle = mpLayout->getStyleFromStylesheet(mStyleName);

        if(mpStyle == NULL)
        {
            throwError(OperationNotifier::Operation::BUG, "Cannot find style with name at initialization of element: " + styleName);

            // Ok, try to rescue by getting default style. Should be NEVER necessary
            mpStyle = mpLayout->getStyleFromStylesheet(DEFAULT_STYLE_NAME);
        }
    }

    Element::~Element()
    {
        // Nothing to do so far
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

    void Element::setStyle(std::string styleName)
    {
        Style const * pStyle = mpLayout->getStyleFromStylesheet(styleName);

        if(pStyle != NULL)
        {
            mpStyle = pStyle;
        }
        else
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Cannot find style with name: " + styleName);
        }
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
                        element->setActivity(true, fade);
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
                    element->setActivity(false, fade);
                }
            }
        }
    }

    void Element::setDimming(bool dimming)
    {
        mDimming = dimming;
    }

    bool Element::isDimming() const
    {
        return mDimming;
    }

    void Element::setMarking(bool marking, int depth)
    {
        mMarking = marking;

        // Affect children
        if (depth != 0)
        {
            // Only decrement positive values
            if (depth > 0)
            {
                depth--;
            }

            // Tell children about it
            for (const std::unique_ptr<Element>& element : mChildren)
            {
                element->setMarking(marking, depth);
            }
        }
    }

    bool Element::isMarking() const
    {
        return mMarking;
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
        mFullDrawMatrix = calculateDrawMatrix(
                            mpLayout->getLayoutWidth(),
                            mpLayout->getLayoutHeight(),
                            mX,
                            mY,
                            mWidth,
                            mHeight);
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

    void Element::setHiding(bool hidden)
    {
        mHidden = hidden;
    }

    float Element::update(float tpf, float alpha, Input* pInput, float dim)
    {
        // *** OWN UPDATING ***

        // Activity animationa
        mActivity.update(tpf / mpLayout->getConfig()->animationDuration, !mActive);

        // Save current alpha (already animated by layout or other element)
        mAlpha = alpha;

        // Check wether cursor is over element
        bool penetrated = penetratedByInput(pInput);

        // Dimming
        if (mForceUndim)
        {
            // Undim it
            mDim.update(-tpf / mpLayout->getConfig()->dimDecreaseDuration);
        }
        else if (mDimming)
        {
            if (penetrated)
            {
                // Undim it
                mDim.update(-tpf / mpLayout->getConfig()->dimDecreaseDuration);
            }
            else
            {
                // Dim it
                mDim.update(tpf / mpLayout->getConfig()->dimIncreaseDuration);
            }
        }
        else
        {
            // Use dimming value of parent
            mDim.setValue(dim);
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
            // Scale it down, because could have been set by using scale from special update (children!)
            mAdaptiveScale.update(-tpf / mpLayout->getConfig()->adaptiveScaleDecreaseDuration);
        }

        // Marking
        mMark.update(tpf / mpLayout->getConfig()->animationDuration, !mMarking);

        // Update replaced element if there is some
        if (mupReplacedElement.get() != NULL)
        {
            float replacedAlpha = mAlpha * (mupReplacedElement->getAlpha()
                - (tpf / mpLayout->getConfig()->animationDuration));
            replacedAlpha = clamp(replacedAlpha, 0, 1);
            mupReplacedElement->update(tpf, replacedAlpha, NULL, mDim.getValue());

            // Check, whether replacement is still visible
            if (replacedAlpha <= 0)
            {
                // Give it to layout for destruction at end of frame
                mpFrame->commitDyingReplacedElement(std::move(mupReplacedElement));
            }
        }

        // *** SUBCLASS METHOD UPDATING ***

        // Use activity and alpha to check whether input may be used by subclasses
        Input* pSubInput;
        if (mAlpha < 1 || !mActive)
        {
            pSubInput = NULL;
        }
        else
        {
            pSubInput = pInput;
        }

        // Call specialized update of subclasses
        float specialAdaptiveScale = specialUpdate(tpf, pSubInput);

        // After subclass has used input, mark it as consumed if necessary
        if(penetrated && mayConsumeInput())
        {
            pInput->gazeUsed = true;
        }

        // *** ADAPTIVE SCALE RETURNING ***

        // Decide, which adaptive scale to save. Own adaptive scale decreases if adaptive scaling is deactivated!
        // If it would be not saved in member, nobody would know about it at rendering.
        mAdaptiveScale.setValue(std::max(mAdaptiveScale.getValue(), specialAdaptiveScale));

        // Return adaptive scale
        return mAdaptiveScale.getValue();
    }

    void Element::draw() const
    {
        // Only draw if visible
        if (mAlpha > 0 && !mHidden)
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

        mDim.setValue(0);
        mForceUndim = false;

        mAdaptiveScale.setValue(0);

        // TODO: one could reset marking, too

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

    bool Element::penetratedByInput(Input const * pInput) const
    {
        // Check whether gaze is upon element
        if (pInput != NULL && !pInput->gazeUsed)
        {
            if (pInput->gazeX >= mX
                && pInput->gazeX <= mX + mWidth
                && pInput->gazeY >= mY
                && pInput->gazeY <= mY + mHeight)
            {
                return true;
            }
        }
        return false;
    }
}
