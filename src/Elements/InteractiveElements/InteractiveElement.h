//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Interactive element is an extened element that supports highlighting,
// interaction, selection and notifications.

#ifndef INTERACTIVE_ELEMENT_H_
#define INTERACTIVE_ELEMENT_H_

#include "Elements/NotifierElement.h"
#include "Elements/NotifierTemplate.h"

namespace eyegui
{
    class InteractiveElement : public NotifierElement
    {
    public:

        // Constructors
        InteractiveElement(
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
            bool adaptiveScaling);

        // Destructor
        virtual ~InteractiveElement() = 0;

        // Check, whether highlighted
        bool isHighlighted() const;

        // Interaction
        void interact();

        // Highlight button, returns whether successful
        void highlight(bool doHighlight);

        // Select button, returns whether successful
        void select(bool doSelect);

        // Tries to fetch next interactive element for selecting, returns NULL if fails
        virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Reset filld by subclasses
        virtual void specialReset();

        // Implemented by subclasses
        virtual bool mayConsumeInput();

        // Interaction fill by subclasses
        virtual void specialInteract() = 0;

        // Members
        LerpValue mHighlight;
        bool mIsHighlighted;
        LerpValue mSelection;
        bool mIsSelected;
        RenderItem const * mpHighlightItem;
        RenderItem const * mpSelectionItem;
    };
}

#endif // INTERACTIVE_ELEMENT_H_
