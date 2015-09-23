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
        Layout* pLayout,
        Frame * pFrame,
        AssetManager* pAssetManager,
        float relativeScale,
        float border,
        RelativeScaling relativeScaling,
        Alignment alignment,
        float padding,
        float innerBorder,
        float separator) : Block(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            relativeScale,
            border)
    {
        mType = Type::STACK;

        // Fill members
        mRelativeScaling = relativeScaling;
        mAlignment = alignment;
        mPadding = padding;
        mInnerBorder = innerBorder;
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

    void Stack::specialUpdate(float tpf, Input* pInput)
    {
        // Update the elements
        for (std::unique_ptr<Element>& element : mChildren)
        {
            element->update(tpf, mAlpha, pInput);
        }

        // Super call after children (may consume input first)
        Block::specialUpdate(tpf, pInput);
    }

    void Stack::specialDraw() const
    {
        // Super call
        Block::specialDraw();

        // Draw the elements
        for (const std::unique_ptr<Element>& element : mChildren)
        {
            element->draw();
        }

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
        return Block::internalNextInteractiveElement(NULL);
    }

    void Stack::specialTransformAndSize()
    {
        // Super call
        Block::specialTransformAndSize();

        // Use inner border
        int usedBorder;
        int innerX, innerY, innerWidth, innerHeight;
        if (getOrientation() == Element::Orientation::HORIZONTAL)
        {
            usedBorder = (int)((float)mHeight * mInnerBorder);
        }
        else
        {
            usedBorder = (int)((float)mWidth * mInnerBorder);
        }
        innerX = mX + usedBorder / 2;
        innerY = mY + usedBorder / 2;
        innerWidth = mWidth - usedBorder;
        innerHeight = mHeight - usedBorder;

        // Calculate separator sizes and adjust inner sizes
        int separatorCount = (int)mChildren.size() - 1;
        int separatorSize = 0;
        std::vector<int> separatorPositions;

        // Only use separators, if wished or reasonable
        if (mSeparator > 0 && separatorCount >= 1)
        {
            // Test even if enough pixels are there to render the separators
            if (getOrientation() == Element::Orientation::HORIZONTAL
                && separatorCount <= innerWidth)
            {
                separatorSize = (int)(innerWidth * mSeparator);
                separatorSize = separatorSize < 1 ? 1 : separatorSize;
                innerWidth -= separatorSize * separatorCount;
            }
            else if (separatorCount <= innerHeight)
            {
                separatorSize = (int)(innerHeight * mSeparator);
                separatorSize = separatorSize < 1 ? 1 : separatorSize;
                innerHeight -= separatorSize * separatorCount;
            }
        }

        // Get all relative scales together if necessary
        float completeScale = 0;
        float maxRelativeScale = 0;
        for (const std::unique_ptr<Element>& element : mChildren)
        {
            float relativeScale = element->getRelativeScale();
            completeScale += relativeScale;
            maxRelativeScale = maxRelativeScale < relativeScale ? relativeScale : maxRelativeScale;
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
                    localElemWidth = innerWidth - sumElemWidth;
                }
                else
                {
                    // Use relative scale
                    localElemWidth = (int)((float)innerWidth
                        * (element->getRelativeScale() / completeScale));
                    sumElemWidth += localElemWidth;
                }

                // Element height
                if (mRelativeScaling == RelativeScaling::BOTH_AXES)
                {
                    localElemHeight = (int)((float)innerHeight
                        * (element->getRelativeScale() / maxRelativeScale));
                }
                else
                {
                    localElemHeight = innerHeight;
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
            int usedPadding = (int)((float)(innerWidth - sumUsedWidth) * mPadding);
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
                int deltaY = innerHeight - usedHeights[i];
                int offsetX;

                int finalX, finalY, finalWidth, finalHeight;

                // Do alignment specific calculations
                switch (mAlignment)
                {
                case Alignment::FILL:
                    deltaX = elemWidths[i] - usedWidths[i];
                    offsetX = innerX;
                    break;
                case Alignment::TAIL:
                    deltaX = usedElemPadding;
                    offsetX = innerX;
                    break;
                case Alignment::HEAD:
                    deltaX = usedElemPadding;
                    offsetX = innerX + (innerWidth - (sumUsedWidth + usedPadding));
                    break;
                default: // Alignment::CENTER
                    deltaX = usedElemPadding;
                    offsetX = innerX + (innerWidth - (sumUsedWidth + usedPadding)) / 2;
                    break;
                }

                // Those values are for all alignments the same
                finalX = usedElemX;
                finalY = innerY + (deltaY / 2);
                finalWidth = usedWidths[i];
                finalHeight = usedHeights[i];

                // Calculate the now used space for next element
                usedElemX = finalX + finalWidth + deltaX;

                // Separators
                if (separatorSize > 0)
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
                    localElemWidth = (int)((float)innerWidth
                        * (element->getRelativeScale() / maxRelativeScale));
                }
                else
                {
                    localElemWidth = innerWidth;
                }

                // Element height
                if (elementNumber == mChildren.size())
                {
                    // Fill stack with last element
                    localElemHeight = innerHeight - sumElemHeight;
                }
                else
                {
                    // Use relative scale
                    localElemHeight = (int)((float)innerHeight
                        * (element->getRelativeScale() / completeScale));
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
            int usedPadding = (int)((float)(innerHeight - sumUsedHeight) * mPadding);
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
                int deltaX = innerWidth - usedWidths[i];
                int deltaY;
                int offsetY;

                int finalX, finalY, finalWidth, finalHeight;

                // Do alignment specific calculations
                switch (mAlignment)
                {
                case Alignment::FILL:
                    deltaY = elemHeights[i] - usedHeights[i];
                    offsetY = innerY;
                    break;
                case Alignment::TAIL:
                    deltaY = usedElemPadding;
                    offsetY = innerY;
                    break;
                case Alignment::HEAD:
                    deltaY = usedElemPadding;
                    offsetY = innerY + (innerHeight - (sumUsedHeight + usedPadding));
                    break;
                default: // Alignment::CENTER
                    deltaY = usedElemPadding;
                    offsetY = innerY + (innerHeight - (sumUsedHeight + usedPadding)) / 2;
                    break;
                }

                // Those values are for all alignments the same
                finalX = innerX + (deltaX / 2);
                finalY = usedElemY;
                finalWidth = usedWidths[i];
                finalHeight = usedHeights[i];

                // Calculate the now used space for next element
                usedElemY = finalY + finalHeight + deltaY;

                // Separators
                if (separatorSize > 0)
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

        // Only think about separators if necessary
        if (mSeparator > 0 && separatorCount >= 1)
        {
            // Calculate draw matrices of separators using new data
            mSeparatorDrawMatrices.clear();

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

            for (int i = 0; i < separatorCount; i++)
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
