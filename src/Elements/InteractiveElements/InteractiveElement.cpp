//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "InteractiveElement.h"

#include "Layout.h"
#include "src/Utilities/Helper.h"

namespace eyegui
{
    InteractiveElement::InteractiveElement(
        std::string id,
        std::string styleName,
        Element* pParent,
        Layout const * pLayout,
        Frame* pFrame,
        AssetManager* pAssetManager,
        NotificationQueue* pNotificationQueue,
        RenderingMask renderingMask,
        float relativeScale,
        float border,
        bool dimming,
        bool adaptiveScaling) : NotifierElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            renderingMask,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::INTERACTIVE_ELEMENT;

        // Filling member
        mHighlight.setValue(0);
        mIsHighlighted = false;
        mSelection.setValue(0);
        mIsSelected = false;

        // Render items
        mpHighlightItem = mpAssetManager->fetchRenderItem(shaders::Type::HIGHLIGHT, meshes::Type::QUAD);
        mpSelectionItem = mpAssetManager->fetchRenderItem(shaders::Type::SELECTION, meshes::Type::QUAD);
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

    InteractiveElement* InteractiveElement::internalNextInteractiveElement(Element const * pCaller)
    {
        return this;
    }

    void InteractiveElement::specialDraw() const
    {
        // Should be called by subclasses after personal drawing

        // Draw highlight
        if(mHighlight.getValue() > 0)
        {
            mpHighlightItem->bind();
            mpHighlightItem->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpHighlightItem->getShader()->fillValue("highlightColor", getStyle()->highlightColor);
            mpHighlightItem->getShader()->fillValue("highlight", mHighlight.getValue());
            mpHighlightItem->getShader()->fillValue("time", mpLayout->getAccPeriodicTime());
            mpHighlightItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpHighlightItem->getShader()->fillValue("mask", 0); // Mask is always in slot 0
            mpHighlightItem->draw();
        }

        // Draw selection
        if(mSelection.getValue() > 0)
        {
            mpSelectionItem->bind();
            mpSelectionItem->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpSelectionItem->getShader()->fillValue("selectionColor", getStyle()->selectionColor);
            mpSelectionItem->getShader()->fillValue("selection", mSelection.getValue());
            mpSelectionItem->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpSelectionItem->getShader()->fillValue("mask", 0); // Mask is always in slot 0
            mpSelectionItem->draw();
        }
    }

    float InteractiveElement::specialUpdate(float tpf, Input* pInput)
    {
        // Highlight
        mHighlight.update(tpf / mpLayout->getConfig()->animationDuration, !mIsHighlighted);

        // Selection
        mSelection.update(tpf / mpLayout->getConfig()->animationDuration, !mIsSelected);

        // Instant interaction
        bool penetrated = penetratedByInput(pInput);
        if(penetrated && pInput->instantInteraction)
        {
            this->interact();
        }

        return 0;
    }

    void InteractiveElement::specialReset()
    {
        mHighlight.setValue(0);
        mIsHighlighted = false;
        mSelection.setValue(0);
        mIsSelected = false;
    }

    bool InteractiveElement::mayConsumeInput()
    {
        return true;
    }
}
