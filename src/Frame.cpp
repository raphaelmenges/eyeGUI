//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Frame.h"

#include "Layout.h"
#include "Helper.h"

namespace eyegui
{
    Frame::Frame(
        Layout * const pLayout,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY)
    {
        // Initialize members
        mpLayout = pLayout;
        mAlpha = 1;
        mVisible = true;
        mupRoot = NULL;
        mResizeNecessary = true;
        mRelativePositionX = relativePositionX;
        mRelativePositionY = relativePositionY;
        mRelativeSizeX = relativeSizeX;
        mRelativeSizeY = relativeSizeY;
    }

    Frame::~Frame()
    {
        // Nothing to do
    }

    void Frame::update(float tpf, float alpha, Input* pInput)
    {
        // *** OWN UPDATE ***

        // Update own alpha
        if (mVisible)
        {
            mAlpha += tpf / mpLayout->getConfig()->animationDuration;
        }
        else
        {
            mAlpha -= tpf / mpLayout->getConfig()->animationDuration;
        }
        mAlpha = clamp(mAlpha, 0, 1);

        // Combine own alpha with layout's
        float combinedAlpha = mAlpha * alpha;

        // Update root only if own alpha greater zero
        if (combinedAlpha > 0)
        {
            // Do not use input if still fading
            if (combinedAlpha < 1)
            {
                pInput = NULL;
            }

            // Update front elements (other way than expected because inner ones are added first)
            for (int i = 0; i < mFrontElements.size(); i++)
            {
                // Alpha value final, do not multiply with mAlpha or combined alpha
                Element* pElement = mFrontElements[i];
                pElement->update(tpf, mFrontElementAlphas[pElement], pInput);
            }

            // Update standard elements
            mupRoot->update(tpf, combinedAlpha, pInput);
        }

        // *** DELETION OF REPLACED ELEMENTS ***

        // Delete replaced elements which has been only kept alive for notifications for one frame
        mDyingReplacedElements.clear();

    }

    void Frame::draw() const
    {
        // Use alpha because while fading it should still draw
        if (mAlpha > 0)
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

    void Frame::resize(bool force)
    {
        if (mAlpha > 0  || force)
        {
            // Fetch values from layout
            int layoutWidth = mpLayout->getLayoutWidth();
            int layoutHeight = mpLayout->getLayoutHeight();

            // Calculate available width and height
            int width = mRelativeSizeX * layoutWidth;
            int height = mRelativeSizeY * layoutHeight;
            int usedWidth, usedHeight;

            // Show attached element centered
            mupRoot->evaluateSize(width, height, usedWidth, usedHeight);
            float deltaX = (width - usedWidth) / 2;
            float deltaY = (height - usedHeight) / 2;
            mupRoot->transformAndSize(
                mRelativePositionX * layoutWidth + deltaX,
                mRelativePositionY * layoutHeight + deltaY,
                usedWidth,
                usedHeight);
            mResizeNecessary = false;
        }
        else
        {
            mResizeNecessary = true;
        }
    }

    void Frame::attachRoot(std::unique_ptr<Element> upElement)
    {
        mupRoot = std::move(upElement);
        resize();
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

    void Frame::setVisibility(bool visible, bool setImmediately)
    {
        // TODO: not accesible through interface

        mVisible = visible;

        // If visible now and resize is necessary, resize!
        if (mVisible && mResizeNecessary)
        {
            resize(true);
        }

        if (setImmediately)
        {
            if (mVisible)
            {
                mAlpha = 1;
            }
            else
            {
                mAlpha = 0;
            }
        }
    }

    InteractiveElement* Frame::getFirstInteractiveElement()
    {
        mupRoot->nextInteractiveElement();
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
}
