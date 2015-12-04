//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Stack.h"

#include "Layout.h"

namespace eyegui
{
    Stack::Stack(
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
        bool consumeInput,
        float innerBorder,
        bool showBackground,
        RelativeScaling relativeScaling,
        Alignment alignment,
        float padding,
        float separator) : Container(
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
            consumeInput,
            innerBorder,
            showBackground)
    {
        mType = Type::STACK;

        // Fill members
        mRelativeScaling = relativeScaling;
        mAlignment = alignment;
        mPadding = padding;
        mSeparator = separator;
        mpSeparator = mpAssetManager->fetchRenderItem(
            shaders::Type::SEPARATOR,
            meshes::Type::QUAD);
    }

    Stack::~Stack()
    {
        // Nothing to do so far
    }

    void Stack::attachElement(std::unique_ptr<Element> upElement)
    {
        mChildren.push_back(std::move(upElement));
    }

    void Stack::specialDraw() const
    {
        // Super call
        Container::specialDraw();

        // Draw separators
        if (mSeparatorDrawMatrices.size() > 0 && getStyle()->separatorColor.a > 0)
        {
            // Bind render item before setting values and drawing
            mpSeparator->bind();

            // Fill color to shader
            mpSeparator->getShader()->fillValue(
                "separatorColor",
                getStyle()->separatorColor);

            // Fill alpha
            mpSeparator->getShader()->fillValue("alpha", mAlpha);

            // Fill dimming
            mpSeparator->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpSeparator->getShader()->fillValue("dimming", mDimming.getValue());

            for (int i = 0; i < mSeparatorDrawMatrices.size(); i++)
            {
                // Fill matrix in shader
                mpSeparator->getShader()->fillValue("matrix", mSeparatorDrawMatrices[i]);

                // Draw render item
                mpSeparator->draw();
            }
        }
    }

    InteractiveElement* Stack::internalNextInteractiveElement(Element const * pChildCaller)
    {
        int start = 0;
        bool startFound = false;

        if (pChildCaller == NULL)
        {
            // No child calling from inside the stadck, so start directly at zero
            startFound = true;
        }

        // Try to find caller in stack
        int index = 0;

        for (std::unique_ptr<Element>& upChild : mChildren)
        {
            if (startFound)
            {
                // Try that element
                return upChild->internalNextInteractiveElement(NULL);
            }
            else
            {
                // Compare with caller
                if (upChild.get() == pChildCaller)
                {
                    start = index;
                    startFound = true;
                }
            }

            index++;
        }

        // Super call
        return Container::internalNextInteractiveElement(NULL);
    }

    void Stack::specialTransformAndSize()
    {
        // Super call
        Container::specialTransformAndSize();

        // Calculate separator sizes and adjust inner sizes
        int separatorCount = (int)mChildren.size() - 1;
        int separatorSize = 0;
        std::vector<int> separatorPositions;

        // Only use separators, if wished or reasonable
        if (mSeparator > 0 && separatorCount >= 1)
        {
            // Test even if enough pixels are there to render the separators
            if (getOrientation() == Element::Orientation::HORIZONTAL
                && separatorCount <= mInnerWidth)
            {
                separatorSize = (int)(mInnerWidth * mSeparator);
                separatorSize = separatorSize < 1 ? 1 : separatorSize;
                mInnerWidth -= separatorSize * separatorCount;
            }
            else if (separatorCount <= mInnerHeight)
            {
                separatorSize = (int)(mInnerHeight * mSeparator);
                separatorSize = separatorSize < 1 ? 1 : separatorSize;
                mInnerHeight -= separatorSize * separatorCount;
            }
        }

        // Get all dynamic scales together if necessary
        float completeScale = 0;
        float maxDynamicScale = 0;
        for (const std::unique_ptr<Element>& element : mChildren)
        {
            float dynamicScale = element->getDynamicScale();
            completeScale += dynamicScale;
            maxDynamicScale = std::max(dynamicScale, maxDynamicScale);
        }

        // Determine direction of stacking
        if (getOrientation() == Element::Orientation::HORIZONTAL)
        {
            // Horizontal
            int usedElemX = 0;
            int sumElemWidth = 0;
            int sumUsedWidth = 0;
            int sumUsedHeight = 0;
            std::vector<int> elemWidths;

            // Collect used size
            std::vector<int> usedWidths, usedHeights;
            int elementNumber = 1;
            for (const std::unique_ptr<Element>& element : mChildren)
            {
                int usedWidth, usedHeight;
                int localElemWidth;
                int localElemHeight;

                // Element width
                if (elementNumber == mChildren.size())
                {
                    // Fill stack with last element
                    localElemWidth = mInnerWidth - sumElemWidth;
                }
                else
                {
                    // Use dynamic scale
                    localElemWidth = (int)((float)mInnerWidth
                        * (element->getDynamicScale() / completeScale));
                    sumElemWidth += localElemWidth;
                }

                // Element height
                if (mRelativeScaling == RelativeScaling::BOTH_AXES)
                {
                    localElemHeight = (int)((float)mInnerHeight
                        * (element->getDynamicScale() / maxDynamicScale));
                }
                else
                {
                    localElemHeight = mInnerHeight;
                }
                elemWidths.push_back(localElemWidth);
                element->evaluateSize(localElemWidth, localElemHeight, usedWidth, usedHeight);
                usedWidths.push_back(usedWidth);
                usedHeights.push_back(usedHeight);
                sumUsedWidth += usedWidth;
                sumUsedHeight += usedHeight;

                // Next looping
                elementNumber++;
            }

            // Alignment
            int i = 0;
            int usedPadding = (int)((float)(mInnerWidth - sumUsedWidth) * mPadding);
            int usedElemPadding = usedPadding / (int)mChildren.size();

            // No padding when alignment is filled
            if (mAlignment == Alignment::FILL)
            {
                usedPadding = 0;
                usedElemPadding = 0;
            }

            // Determine final values and assign them
            for (const std::unique_ptr<Element>& element : mChildren)
            {
                int deltaX;
                int deltaY = mInnerHeight - usedHeights[i];
                int offsetX;

                int finalX, finalY, finalWidth, finalHeight;

                // Do alignment specific calculations
                switch (mAlignment)
                {
                case Alignment::FILL:
                    deltaX = elemWidths[i] - usedWidths[i];
                    offsetX = mInnerX;
                    break;
                case Alignment::TAIL:
                    deltaX = usedElemPadding;
                    offsetX = mInnerX;
                    break;
                case Alignment::HEAD:
                    deltaX = usedElemPadding;
                    offsetX = mInnerX + (mInnerWidth - (sumUsedWidth + usedPadding));
                    break;
                default: // Alignment::CENTER
                    deltaX = usedElemPadding;
                    offsetX = mInnerX + (mInnerWidth - (sumUsedWidth + usedPadding)) / 2;
                    break;
                }

                // Those values are for all alignments the same
                finalX = usedElemX;
                finalY = mInnerY + (deltaY / 2);
                finalWidth = usedWidths[i];
                finalHeight = usedHeights[i];

                // Calculate the now used space for next element
                usedElemX = finalX + finalWidth + deltaX;

                // Separators (only add new ones if not last element)
                if (separatorSize > 0 && separatorPositions.size() < separatorCount)
                {
                    separatorPositions.push_back(usedElemX + offsetX);
                    usedElemX += separatorSize;
                }

                // Finalize x coordinate for current element
                finalX += (deltaX / 2 + offsetX);

                // Tell element about it
                element->transformAndSize(finalX, finalY, finalWidth, finalHeight);

                // Next looping
                i++;
            }
        }
        else
        {
            // Vertical
            int usedElemY = 0;
            int sumElemHeight = 0;
            int sumUsedWidth = 0;
            int sumUsedHeight = 0;
            std::vector<int> elemHeights;

            // Collect used size
            std::vector<int> usedWidths, usedHeights;
            int elementNumber = 1;
            for (const std::unique_ptr<Element>& element : mChildren)
            {
                int usedWidth, usedHeight;
                int localElemWidth;
                int localElemHeight;

                // Element width
                if (mRelativeScaling == RelativeScaling::BOTH_AXES)
                {
                    localElemWidth = (int)((float)mInnerWidth
                        * (element->getDynamicScale() / maxDynamicScale));
                }
                else
                {
                    localElemWidth = mInnerWidth;
                }

                // Element height
                if (elementNumber == mChildren.size())
                {
                    // Fill stack with last element
                    localElemHeight = mInnerHeight - sumElemHeight;
                }
                else
                {
                    // Use dynamic scale
                    localElemHeight = (int)((float)mInnerHeight
                        * (element->getDynamicScale() / completeScale));
                    sumElemHeight += localElemHeight;
                }

                elemHeights.push_back(localElemHeight);
                element->evaluateSize(localElemWidth, localElemHeight, usedWidth, usedHeight);
                usedWidths.push_back(usedWidth);
                usedHeights.push_back(usedHeight);
                sumUsedWidth += usedWidth;
                sumUsedHeight += usedHeight;

                // Next looping
                elementNumber++;
            }

            // Alignment
            int i = 0;
            int usedPadding = (int)((float)(mInnerHeight - sumUsedHeight) * mPadding);
            int usedElemPadding = usedPadding / (int)mChildren.size();

            // No padding when alignment is filled
            if (mAlignment == Alignment::FILL)
            {
                usedPadding = 0;
                usedElemPadding = 0;
            }

            // Determine final values and assign them
            for (const std::unique_ptr<Element>& element : mChildren)
            {
                int deltaX = mInnerWidth - usedWidths[i];
                int deltaY;
                int offsetY;

                int finalX, finalY, finalWidth, finalHeight;

                // Do alignment specific calculations
                switch (mAlignment)
                {
                case Alignment::FILL:
                    deltaY = elemHeights[i] - usedHeights[i];
                    offsetY = mInnerY;
                    break;
                case Alignment::TAIL:
                    deltaY = usedElemPadding;
                    offsetY = mInnerY;
                    break;
                case Alignment::HEAD:
                    deltaY = usedElemPadding;
                    offsetY = mInnerY + (mInnerHeight - (sumUsedHeight + usedPadding));
                    break;
                default: // Alignment::CENTER
                    deltaY = usedElemPadding;
                    offsetY = mInnerY + (mInnerHeight - (sumUsedHeight + usedPadding)) / 2;
                    break;
                }

                // Those values are for all alignments the same
                finalX = mInnerX + (deltaX / 2);
                finalY = usedElemY;
                finalWidth = usedWidths[i];
                finalHeight = usedHeights[i];

                // Calculate the now used space for next element
                usedElemY = finalY + finalHeight + deltaY;

                // Separators (only add new ones if not last element)
                if (separatorSize > 0 && separatorPositions.size() < separatorCount)
                {
                    separatorPositions.push_back(usedElemY + offsetY);
                    usedElemY += separatorSize;
                }

                // Finalize y coordinate for current element
                finalY += (deltaY / 2 + offsetY);

                // Tell element about it
                element->transformAndSize(finalX, finalY, finalWidth, finalHeight);

                // Next looping
                i++;
            }
        }

        // Calculate draw matrices of separators using new data
        mSeparatorDrawMatrices.clear();

        // Only think about separators if necessary
        if (mSeparator > 0 && separatorCount >= 1)
        {
            // Calculate correct transformation
            int separatorWidth, separatorHeight;

            // Scale depending on orientation
            if (getOrientation() == Element::Orientation::HORIZONTAL)
            {
                separatorWidth = separatorSize;
                separatorHeight = mHeight;
            }
            else
            {
                separatorWidth = mWidth;
                separatorHeight = separatorSize;
            }

            for (int i = 0; i < separatorPositions.size(); i++)
            {
                // Translation depending on orientation
                if (getOrientation() == Element::Orientation::HORIZONTAL)
                {
                    mSeparatorDrawMatrices.push_back(
                        calculateDrawMatrix(
                            separatorPositions[i],
                            mY,
                            separatorWidth,
                            separatorHeight));
                }
                else
                {
                    mSeparatorDrawMatrices.push_back(
                        calculateDrawMatrix(
                            mX,
                            separatorPositions[i],
                            separatorWidth,
                            separatorHeight));
                }
            }
        }
    }
}
