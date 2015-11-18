//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Frame.h"

#include "Layout.h"
#include "Helper.h"

#include <cmath>

namespace eyegui
{
    Frame::Frame(
        Layout * const pLayout,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY) : Object()
    {
        // Initialize members
        mpLayout = pLayout;
        mFrameAlpha.setValue(1);
        mCombinedAlpha = 1;
        mRemovedFadingAlpha = 1;
        mVisible = true;
        mupRoot = NULL;
        mResizeNecessary = true;
        mRelativePositionX = relativePositionX;
        mRelativePositionY = relativePositionY;
        mRelativeSizeX = relativeSizeX;
        mRelativeSizeY = relativeSizeY;
        mRemoved = false;

        clampSize();
    }

    Frame::~Frame()
    {
        // Nothing to do
    }

    int Frame::getX() const
    {
        return (int)(mRelativePositionX * mpLayout->getLayoutWidth());
    }
    int Frame::getY() const
    {
        return (int)(mRelativePositionY * mpLayout->getLayoutHeight());
    }

    int Frame::getWidth() const
    {
        return (int)(mRelativeSizeX * mpLayout->getLayoutWidth());
    }

    int Frame::getHeight() const
    {
        return (int)(mRelativeSizeY * mpLayout->getLayoutHeight());
    }

    float Frame::getRelativePositionOnLayoutX() const
    {
        return mRelativePositionX;
    }

    float Frame::getRelativePositionOnLayoutY() const
    {
        return mRelativePositionY;
    }

    float Frame::getRelativeSizeOnLayoutX() const
    {
        return mRelativeSizeX;
    }

    float Frame::getRelativeSizeOnLayoutY() const
    {
        return mRelativeSizeY;
    }

    void Frame::update(float tpf, float alpha, Input* pInput)
    {
        // *** RESIZING ***
        internalResizing();

        // *** OWN UPDATE ***

        // Update own alpha
        mFrameAlpha.update(tpf / mpLayout->getConfig()->animationDuration, !mVisible);

        // Combine own alpha with layout's
        mCombinedAlpha = mFrameAlpha.getValue() * mRemovedFadingAlpha * alpha;

         // *** OWN ROOT AND FRONT ELEMENTS ***

        // Update root only if own alpha greater zero
        if (mCombinedAlpha > 0)
        {
            // Do not use input if still fading
            if (mCombinedAlpha < 1)
            {
                pInput = NULL;
            }

            // Update front elements (other way than expected because inner ones are added first)
            for (int i = 0; i < mFrontElements.size(); i++)
            {
                // Alpha value final, do not multiply with mAlpha or combined alpha
                Element* pElement = mFrontElements[i];
                pElement->update(tpf, mFrontElementAlphas[pElement], pInput, 0); // TODO: Intelligent decision to deactivate dimming here?
            }

            // Update standard elements
            float adaptiveScale = mupRoot->update(tpf, mCombinedAlpha, pInput, 0);
            if (adaptiveScale > 0)
            {
                mResizeNecessary = true;
            }
        }

        // *** DELETION OF REPLACED ELEMENTS ***

        // Delete replaced elements which has been only kept alive for notifications for one frame
        mDyingReplacedElements.clear();

    }

    void Frame::draw() const
    {
        // Use alpha because while fading it should still draw
        if (mCombinedAlpha > 0)
        {
            // Draw standard elements
            mupRoot->draw();

            // Draw front elements (other way than expected because inner ones are added first)
            for (int i = (int)mFrontElements.size() - 1; i >= 0; i--)
            {
                Element const * pElement = mFrontElements[i];
                pElement->draw();
            }
        }
    }

    void Frame::makeResizeNecessary()
    {
        mResizeNecessary = true;
    }

    void Frame::attachRoot(std::unique_ptr<Element> upElement)
    {
        // Initial resize is done at first update
        mupRoot = std::move(upElement);
    }

    std::unique_ptr<Element> Frame::replaceRoot(std::unique_ptr<Element> upElement)
    {
        auto upOldRoot = std::move(mupRoot);
        attachRoot(std::move(upElement));
        return std::move(upOldRoot);
    }

    void Frame::registerFrontElementForUpdateAndDraw(Element* pElement, bool visible)
    {
        mFrontElements.push_back(pElement);

        if (visible)
        {
            mFrontElementAlphas[pElement] = 1;
        }
        else
        {
            mFrontElementAlphas[pElement] = 0;
        }
    }

    void Frame::setFrontElementAlpha(Element* pElement, float alpha)
    {
        mFrontElementAlphas[pElement] = alpha;
    }

    void Frame::commitDyingReplacedElement(std::unique_ptr<Element> upElement)
    {
        mDyingReplacedElements.push_back(std::move(upElement));
    }

    void Frame::setVisibility(bool visible, bool fade)
    {
        mVisible = visible;

        if (!fade)
        {
            if (mVisible)
            {
                mFrameAlpha.setValue(1);
            }
            else
            {
                mFrameAlpha.setValue(0);
            }
        }
    }

    InteractiveElement* Frame::getFirstInteractiveElement() const
    {
        return mupRoot->nextInteractiveElement();
    }

    void Frame::resetElements()
    {
        mupRoot->reset();
    }

    void Frame::removeFrontElementsOfElement(Element* pTarget)
    {
        // Get rid of front element
        for (Element* pFrontElement : mFrontElements)
        {
            if (pFrontElement == pTarget)
            {
                mFrontElementAlphas.erase(pTarget);
                mFrontElements.erase(std::remove(mFrontElements.begin(), mFrontElements.end(), pTarget), mFrontElements.end());
                break; // Iterator is more or less dead now, break must be
            }
        }

        // Also, kill all front element pointer to children
        std::set<Element*> children = pTarget->getAllChildren();
        for (Element* pChild : children)
        {
            for (Element* pFrontElement : mFrontElements)
            {
                if (pFrontElement == pChild)
                {
                    mFrontElementAlphas.erase(pFrontElement);
                    mFrontElements.erase(std::remove(mFrontElements.begin(), mFrontElements.end(), pFrontElement), mFrontElements.end());
                    break; // Iterator is more or less dead now, break must be
                }
            }
        }
    }

    std::set<Element*> Frame::getAllElements() const
    {
        // Get all children of root
        std::set<Element*> elements = mupRoot->getAllChildren();

        // Get the root
        elements.insert(mupRoot.get());

        // Return set
        return elements;
    }

    std::set<std::string> Frame::getAllElementsIds() const
    {
        return mupRoot->getAllChildrensIds();
    }

    void Frame::setRemovedFadingAlpha(float alpha)
    {
        mRemovedFadingAlpha = alpha;
    }

    float Frame::getRemovedFadingAlpha() const
    {
        return mRemovedFadingAlpha;
    }

    void Frame::setRemoved()
    {
        mRemoved = true;
    }

    bool Frame::isRemoved() const
    {
        return mRemoved;
    }

    void Frame::translate(float translateX, float translateY)
    {
        mRelativePositionX += translateX;
        mRelativePositionY += translateY;
        mResizeNecessary = true;
    }

    void Frame::scale(float scaleX, float scaleY)
    {
        mRelativeSizeX *= scaleX;
        mRelativeSizeY *= scaleY;
        clampSize();
        mResizeNecessary = true;
    }

    void Frame::setPosition(float relativePositionX, float relativePositionY)
    {
        mRelativePositionX = relativePositionX;
        mRelativePositionY = relativePositionY;
    }

    void Frame::setSize(float relativeSizeX, float relativeSizeY)
    {
        mRelativeSizeX = relativeSizeX;
        mRelativeSizeY = relativeSizeY;
        clampSize();
        mResizeNecessary = true;
    }

    void Frame::internalResizing()
    {
        if (mResizeNecessary && mCombinedAlpha > 0)
        {
            // Fetch values from layout
            int layoutWidth = mpLayout->getLayoutWidth();
            int layoutHeight = mpLayout->getLayoutHeight();

            // Calculate available width and height
            int width = (int)std::round(mRelativeSizeX * (float)layoutWidth);
            int height = (int)std::round(mRelativeSizeY * (float)layoutHeight);
            int usedWidth, usedHeight;

            // Show attached element centered
            mupRoot->evaluateSize(width, height, usedWidth, usedHeight);
            float deltaX = (width - usedWidth) / 2.0f;
            float deltaY = (height - usedHeight) / 2.0f;
            mupRoot->transformAndSize(
                (int)std::round(mRelativePositionX * (float)layoutWidth + deltaX),
                (int)std::round(mRelativePositionY * (float)layoutHeight + deltaY),
                usedWidth,
                usedHeight);
            mResizeNecessary = false;
        }
    }

    void Frame::clampSize()
    {
        mRelativeSizeX = std::max(mRelativeSizeX, 0.0f);
        mRelativeSizeY = std::max(mRelativeSizeY, 0.0f);
    }
}
