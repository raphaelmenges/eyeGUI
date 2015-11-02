//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Grid.h"

#include "Elements/InteractiveElements/InteractiveElement.h"

namespace eyegui
{
    Grid::Grid(
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
        int rows,
		float innerBorder) : Container(
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
			innerBorder)
    {
        mType = Type::GRID;

        // Initialize members
        mRows = rows;

        // Resizing of vectors to fit row count
        mCellIndices.resize(rows);
        mElementRelativeWidths.resize(rows);
        mElementRelativeHeights.resize(rows);
        mColumns.resize(rows);
    }

    Grid::~Grid()
    {
        // Nothing to do so far
    }

    Element* Grid::getElement(int row, int column)
    {
        return mChildren[(mCellIndices[row][column])].get();
    }

    void Grid::prepareColumns(int row, int columns)
    {
        // Resize vectors by column count
        mCellIndices[row].resize(columns);
        mElementRelativeWidths[row].resize(columns);

        // Save count of columns per row
        mColumns[row] = columns;
    }

    void Grid::attachElement(
        int row,
        int column,
        std::unique_ptr<Element> upElement)
    {
        // First, extract size of children vector as index
        mCellIndices[row][column] = (int)mChildren.size();

        // Become owner of element
        mChildren.push_back(std::move(upElement));
    }

    bool Grid::gridCompletelyFilledBySizes() const
    {
        // Check sum of heights
        float sum = 0;
        for (float height : mElementRelativeHeights)
        {
            sum += height;
        }
        if (sum != 1)
        {
            return false;
        }

        // Check sum of widths
        for (const std::vector<float>& columns : mElementRelativeWidths)
        {
            sum = 0;
            for (float width : columns)
            {
                sum += width;
            }
            if (sum != 1)
            {
                return false;
            }
        }

        return true;
    }

    void Grid::setRelativeHeightOfRow(int row, float height)
    {
        mElementRelativeHeights[row] = height;
    }

    void Grid::setRelativeWidthOfCell(int row, int column, float width)
    {
        mElementRelativeWidths[row][column] = width;
    }

    float Grid::getRelativeHeight(int row) const
    {
        return mElementRelativeHeights[row];
    }

    float Grid::getRelativeWidth(int row, int column) const
    {
        return mElementRelativeWidths[row][column];
    }

    InteractiveElement* Grid::internalNextInteractiveElement(Element const * pChildCaller)
    {
        bool startCellFound = false;

        if (pChildCaller == NULL)
        {
            // No child calling from inside the grid, so start directly at zero
            startCellFound = true;
        }

        // Try to find caller in grid
        for (int row = 0; row < mRows; row++)
        {
            for (int column = 0; column < mColumns[row]; column++)
            {
                Element* pElement = mChildren[(mCellIndices[row][column])].get();

                if (startCellFound)
                {
                    // Try that element
                    return pElement->internalNextInteractiveElement(NULL);
                }
                else
                {
                    // Compare with caller
                    if (pElement == pChildCaller)
                    {
                        startCellFound = true;
                    }
                }
            }
        }

        // Nothing found in grid
        return Container::internalNextInteractiveElement(NULL);
    }

    void Grid::specialTransformAndSize()
    {
        // Super call
        Container::specialTransformAndSize();

		// Calculate dynamic scales
		std::vector<float> medianDynamicScaleInColumns;
		std::vector<float> completeScaleOfColumns;
		float completeScaleOfRows = 0;
		for (int i = 0; i < mRows; i++)
		{
			int columnCount = mColumns[i];
			float medianDynamicScale = 0;
			float completeScale = 0;

			// Go over columns
			for (int j = 0; j < columnCount; j++)
			{
				Element* ptr = mChildren[(mCellIndices[i][j])].get();
				completeScale += ptr->getDynamicScale();
			}

			// Save maximal dynamic scale for this row
			medianDynamicScaleInColumns.push_back(completeScale / columnCount);
			completeScaleOfColumns.push_back(completeScale);
			completeScaleOfRows += completeScale / columnCount;
		}

        // Initialize some values
        float currentRelativeYEnd = 0;
        int elemY = mInnerY;
        int elemWidth, elemHeight = 0;

        // Go over rows and accumulate y
        for (int i = 0; i < mRows; i++)
        {
            // Necessary to calculate height of element
            currentRelativeYEnd += mElementRelativeHeights[i] * (medianDynamicScaleInColumns[i] / completeScaleOfRows) * mRows;

            // Initalize values per row
            int columnCount = mColumns[i];
            float currentRelativeXEnd = 0;
            int elemX = mInnerX;

            // Height is the same for one row
            if ((i + 1) == mRows)
            {
                // Fill until last pixel
                elemHeight = (mInnerHeight + mInnerY) - elemY;
            }
            else
            {
                // Fill until end of grid cell
                elemHeight = (int)(currentRelativeYEnd * mInnerHeight) - elemY + mInnerY;
            }

            // Go over columns and accumulate x
            for (int j = 0; j < columnCount; j++)
            {
				// Pointer to element in cell
				Element* ptr = mChildren[(mCellIndices[i][j])].get();

                // Necessary to calculate width of element
                currentRelativeXEnd += mElementRelativeWidths[i][j] * (ptr->getDynamicScale() / completeScaleOfColumns[i]) * columnCount;

                // Calculate available space
                if ((j + 1) == columnCount)
                {
                    // Fill until last pixel
                    elemWidth = (mInnerWidth + mInnerX) - elemX;
                }
                else
                {
                    // Fill until end of grid cell
                    elemWidth = (int)(currentRelativeXEnd * mInnerWidth) - elemX + mInnerX;
                }

                // Now ask the element, how much space is actually used
                int usedWidth, usedHeight;
                
                ptr->evaluateSize(elemWidth, elemHeight, usedWidth, usedHeight);

                // Recalculate values to place element in center of cell
                int deltaX = elemWidth - usedWidth;
                int deltaY = elemHeight - usedHeight;
                int usedElemX = elemX + (deltaX / 2);
                int usedElemY = elemY + (deltaY / 2);
                int usedElemWidth = elemWidth - deltaX;
                int usedElemHeight = elemHeight - deltaY;

                // Tell element about its new position and size
                ptr->transformAndSize(usedElemX, usedElemY, usedElemWidth, usedElemHeight);

                // Prepare for next round
                elemX += elemWidth;
            }

            // Prepare for next round
            elemY += elemHeight;
        }
    }
}
