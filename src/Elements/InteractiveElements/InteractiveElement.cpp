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
        Layout* pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        float relativeScale,
        float border,
        std::string iconFilepath) : Element(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            relativeScale,
            border)
    {
        mType = Type::INTERACTIVE_ELEMENT;

        // Filling members
        setIcon(iconFilepath);
        mpRenderItem = NULL;
        mHighlight = 0;
        mIsHighlighted = false;
        mSelection = 0;
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

    void InteractiveElement::pipeNotification(Notification notification)
    {
        // Only pipe if visible (so fading elements do not notify)
        if (mAlpha >= 1)
        {
            specialPipeNotification(notification);
        }
    }

    InteractiveElement* InteractiveElement::internalNextInteractiveElement(Element const * pCaller)
    {
        return this;
    }

    void InteractiveElement::specialUpdate(float tpf, Input* pInput)
    {
        // Highlight
        if (mIsHighlighted)
        {
            mHighlight += tpf;
        }
        else
        {
            mHighlight -= tpf;
        }
        mHighlight = clamp(mHighlight, 0, 1);

        // Selection
        if (mIsSelected)
        {
            mSelection += tpf / mpLayout->getConfig()->animationDuration;
        }
        else
        {
            mSelection -= tpf / mpLayout->getConfig()->animationDuration;
        }
        mSelection = clamp(mSelection, 0, 1);
    }

    void InteractiveElement::specialDraw() const
    {
        // Fill shader
        mpRenderItem->getShader()->fillValue("matrix", mDrawMatrix);
        mpRenderItem->getShader()->fillValue("highlight", mHighlight);
        mpRenderItem->getShader()->fillValue("alpha", mAlpha);
        mpRenderItem->getShader()->fillValue("activity", mActivity);
        mpRenderItem->getShader()->fillValue("selection", mSelection);
        mpRenderItem->getShader()->fillValue("color", getStyle()->color);
        mpRenderItem->getShader()->fillValue("highlightColor", getStyle()->highlightColor);
        mpRenderItem->getShader()->fillValue("selectionColor", getStyle()->selectionColor);
        mpRenderItem->getShader()->fillValue("iconColor", getStyle()->iconColor);
        mpRenderItem->getShader()->fillValue("time", mpLayout->getAccPeriodicTime());

        // Bind icon texture
        mpIcon->bind(0);
    }

    void InteractiveElement::specialReset()
    {
        mHighlight = 0;
        mIsHighlighted = false;
        mSelection = 0;
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

    bool InteractiveElement::penetratedByInput(Input const * pInput) const
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
