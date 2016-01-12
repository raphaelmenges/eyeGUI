//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"


// TODO: Testing
#include <iostream>

namespace eyegui
{
    Keyboard::Keyboard(
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
        bool adaptiveScaling) : Element(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::KEYBOARD;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);

        // Fetch render item for keys
        mpKey = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);

        // TODO: Testing
        mKeyCount = 35;
    }

    Keyboard::~Keyboard()
    {
        // Nothing to do
    }

    float Keyboard::specialUpdate(float tpf, Input* pInput)
    {
        // TODO: update keys (maybe do not save keycount but vector with matrices and...vector with letters...)
        return 0;
    }

    void Keyboard::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("backgroundColor", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", mAlpha);
            mpBackground->getShader()->fillValue("activity", mActivity.getValue());
            mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpBackground->getShader()->fillValue("dim", mDim.getValue());
            mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
            mpBackground->getShader()->fillValue("mark", mMark.getValue());
            mpBackground->draw();
        }

        // *** RENDER KEYS ***
        mpKey->bind();
        mpKey->getShader()->fillValue("alpha", mAlpha);
        for(const glm::vec2& rKeyPosition : mKeyPositions)
        {
            glm::mat4 matrix = calculateDrawMatrix(
                mX + (int)(rKeyPosition.x - mKeyRadius),
                mY + (int)(rKeyPosition.y - mKeyRadius),
                (int)(2 * mKeyRadius),
                (int)(2 * mKeyRadius));
            mpKey->getShader()->fillValue("matrix", matrix);

            // Draw the key
            mpKey->draw();
        }
    }

    void Keyboard::specialTransformAndSize()
    {
        // Recalculate key positions
        calculateKeyPositions(mWidth, mHeight, mKeyPositions, mKeyRadius);
    }

    void Keyboard::specialReset()
    {
        // Nothing to do
    }

    bool Keyboard::mayConsumeInput()
    {
        return false;
    }

    void Keyboard::calculateKeyPositions(int availableWidth, int availableHeight, std::vector<glm::vec2>& rPositions, int& rRadius) const
    {
        // Notes:
        // - Everything calculated in pixel space but with origin in middle of available space
        // - Coordinate system of eyeGUI used (origin at top left corner)

        // Initialize some useful variables
        unsigned int ring = 0;
        unsigned int fullyVisibleRing = 0;
        std::vector<IntegerKeyPosition> availablePositions;
        std::vector<IntegerKeyPosition> reservedPositions;
        int halfWidth = availableWidth / 2;
        int halfHeight = availableHeight / 2;

        // Determine start radius for circles
        int currentRadius;
        currentRadius = availableWidth > availableHeight ? availableHeight : availableWidth;
        currentRadius /= 2;

        // Clear positions vector
        rPositions.clear();

        // Do it until enough key positions are available
        bool positionsNeeded = true;
        int startIndexOfAvailable = 0;
        while(positionsNeeded)
        {
            // Add new ring
            addAvailablePositionsOfRing(ring, availablePositions);

            // Reserve available positions
            std::vector<int> indicesOfPositionsToBeDeleted;
            for(int i = startIndexOfAvailable; i < availablePositions.size(); i++)
            {
                // Get available position
                IntegerKeyPosition availPos = availablePositions.at(i);

                // Check, whether available position is inside space
                bool inside = circleInRectangle(
                    -halfWidth,
                    -halfHeight,
                    availableWidth,
                    availableHeight,
                    availPos.first * currentRadius,
                    availPos.second * currentRadius,
                    currentRadius);

                // If it is inside, use it!
                if(inside)
                {
                    // Reserve this position for a key
                    reservedPositions.push_back(availPos);

                    // Remember to delete ist from available positions
                    indicesOfPositionsToBeDeleted.push_back(i);
                }
            }

            // Delete all used positions from available positions
            for(int i = indicesOfPositionsToBeDeleted.size() - 1; i >= 0; i--)
            {
                availablePositions.erase(availablePositions.begin() + indicesOfPositionsToBeDeleted[i]);
            }

            // Check, whether further positions are needed
            positionsNeeded = reservedPositions.size() < mKeyCount;

            // Prepare next run if necessary
            if(positionsNeeded)
            {
                // Check, whether size decrease is necessary or just new ring
                if(indicesOfPositionsToBeDeleted.size() <= 0)
                {
                    // Decrease size so that one ring more fits
                    fullyVisibleRing++;
                    int divisor = (2 * fullyVisibleRing) + 1; // TODO: float?
                    if(availableWidth > availableHeight)
                    {
                        currentRadius = (availableHeight / divisor) / 2;
                    }
                    else
                    {
                        currentRadius = (availableWidth / divisor) / 2;
                    }

                    // Reset index where available positions should be searched
                    startIndexOfAvailable = 0;
                }
                else
                {
                    // Increase ring
                    ring++;
                    startIndexOfAvailable = availablePositions.size();
                }
            }
        }

        // Save reserved positions in output vector in correct coordinates
        for(int i = 0; i < mKeyCount; i++)
        {
            const IntegerKeyPosition& rKeyPosition = reservedPositions[i];
            rPositions.push_back(glm::vec2(
                rKeyPosition.first * currentRadius + halfWidth,
                rKeyPosition.second * currentRadius + halfHeight));
        }

        // Save radius
        rRadius = currentRadius;
    }

    void Keyboard::addAvailablePositionsOfRing(
        unsigned int ring,
        std::vector<IntegerKeyPosition>& rAvailablePositions) const
    {
        // Notes:
        // - First left, then right
        // - First up, then down
        // - Coordinate system of eyeGUI used (origin at top left corner)
        // - Horizontal offset of positions is 2!

        if(ring <= 0)
        {
            // No hexagon, just the middle position
            rAvailablePositions.push_back(IntegerKeyPosition(0,0));
        }
        else
        {
            // Add first line
            rAvailablePositions.push_back(IntegerKeyPosition(-2 * ring, 0));
            rAvailablePositions.push_back(IntegerKeyPosition(2 * ring, 0));

            // Go over height of hexagon ring to handle rest
            for(int h = 1; h <= ring; h++)
            {
                if(h < ring)
                {
                    // Two positions on the sides on top
                    rAvailablePositions.push_back(IntegerKeyPosition((-2 * ring) + h, -2 * h));
                    rAvailablePositions.push_back(IntegerKeyPosition((2 * ring) - h, -2 * h));

                    // Two positions on the sides on bottom
                    rAvailablePositions.push_back(IntegerKeyPosition((-2 * ring) + h, 2 * h));
                    rAvailablePositions.push_back(IntegerKeyPosition((2 * ring) - h, 2 * h));
                }
                else
                {
                    // Add positions on top of hexagon ring
                    for(int i = 0; i <= ring; i++)
                    {
                        rAvailablePositions.push_back(IntegerKeyPosition(-ring + 2 * i, -2 * h));
                    }

                    // Add positions on bottom of hexagon ring
                    for(int i = 0; i <= ring; i++)
                    {
                        rAvailablePositions.push_back(IntegerKeyPosition(-ring + 2 * i, 2 * h));
                    }
                }
            }
        }
    }

    bool Keyboard::circleInRectangle(int rectX, int rectY, int rectWidth, int rectHeight, int circleX, int circleY, int circleRadius) const
    {

        if( circleX - circleRadius >= rectX && circleX + circleRadius < rectX + rectWidth
            && circleY - circleRadius >= rectY && circleY + circleRadius  < rectY + rectHeight)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

}
