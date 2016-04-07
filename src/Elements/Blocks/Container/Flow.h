//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FLOW_H_
#define FLOW_H_

#include "src/Elements/Blocks/Container/Container.h"

// TODO
// - Improve scrolling
// - Use inner width and height
// - Does not work correctly when just button or so is inner element (one has to evaluate used size first)
// - Interface function to change space

namespace eyegui
{
    class Flow : public Container
    {
    public:

        // Constructor
        Flow(
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
            bool adaptiveScaling,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            float innerBorder,
            bool showBackground,
            float space);

        // Destructor
        virtual ~Flow();

        // Attach inner element (must be called exactly one time)
        void attachInnerElement(std::unique_ptr<Element> upElement);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Draw on top of element (used for drawing children over effects like marking)
        virtual void drawOnTop() const;

    private:

        // Transform inner element
        void transformInnerElement();

        // Members
        float mSpace;
        LerpValue mOffset;
    };
}

#endif // FLOW_H_
