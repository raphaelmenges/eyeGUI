//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ProgressBar.h"

#include "src/Layout.h"

namespace eyegui
{
    ProgressBar::ProgressBar(
        std::string id,
        std::vector<std::string> styles,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling,
        bool consumeInput,
        std::string backgroundFilepath,
        ImageAlignment backgroundAlignment,
        float innerBorder,
        Direction direction) : Block(
            id,
            styles,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
            dimming,
            adaptiveScaling,
            consumeInput,
            backgroundFilepath,
            backgroundAlignment,
            innerBorder)
    {
        mType = Type::PROGRESS_BAR;

        // Fill members
        mProgress = 0;
        mDirection = direction;

        // Render time for progress
        mpProgressItem = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
    }

    ProgressBar::~ProgressBar()
    {
        // Nothing to do
    }

    void ProgressBar::setProgress(float progress)
    {
        // Clamp and set member
        mProgress = glm::clamp(progress, 0.f, 1.f);
    }

    void ProgressBar::specialDraw() const
    {
        // Draw background
        Block::specialDraw();

        // Draw progress
        if(mProgress > 0)
        {
            // Calculate draw matrix for progress
            glm::mat4 progressDrawMatrix;

            // Decide the direction
            switch(mDirection)
            {
            case ProgressBar::Direction::LEFT_TO_RIGHT:
                progressDrawMatrix = calculateDrawMatrix(
                    mpLayout->getLayoutWidth(),
                    mpLayout->getLayoutHeight(),
                    mInnerX,
                    mInnerY,
                    (int)(mInnerWidth * mProgress),
                    mInnerHeight);
                break;
            case ProgressBar::Direction::RIGHT_TO_LEFT:
                progressDrawMatrix = calculateDrawMatrix(
                    mpLayout->getLayoutWidth(),
                    mpLayout->getLayoutHeight(),
					(int)(mInnerX + (mInnerWidth * (1.f - mProgress))),
                    mInnerY,
                    (int)(mInnerWidth * mProgress),
                    mInnerHeight);
                break;
            case ProgressBar::Direction::TOP_TO_BOTTOM:
                progressDrawMatrix = calculateDrawMatrix(
                    mpLayout->getLayoutWidth(),
                    mpLayout->getLayoutHeight(),
                    mInnerX,
                    mInnerY,
                    mInnerWidth,
					(int)(mInnerHeight * mProgress));
                break;
            case ProgressBar::Direction::BOTTOM_TO_TOP:
                progressDrawMatrix = calculateDrawMatrix(
                    mpLayout->getLayoutWidth(),
                    mpLayout->getLayoutHeight(),
                    mInnerX,
					(int)(mInnerY  + (mInnerHeight * (1.f - mProgress))),
                    mInnerWidth,
                    (int)(mInnerHeight * mProgress));
                break;
            }

            mpProgressItem->bind();
            mpProgressItem->getShader()->fillValue("matrix", progressDrawMatrix);
            mpProgressItem->getShader()->fillValue("color", getStyleValue(property::Color::Color));
            mpProgressItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpProgressItem->draw();
        }
    }
}
