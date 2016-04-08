//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Flow is like a vertical scrolling box. One can define, how much vertical
// space is reserved for an inner element compared to own height. Update and
// drawing of inner element happens only in the bounds of the flow element.

#ifndef FLOW_H_
#define FLOW_H_

#include "src/Elements/Blocks/Container/Container.h"

namespace eyegui
{
	// Direction of flow
	enum class FlowDirection { HORIZONTAL, VERTICAL };

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
			FlowDirection direction,
            float space);

        // Destructor
        virtual ~Flow();

        // Attach inner element (must be called exactly one time)
        void attachInnerElement(std::unique_ptr<Element> upElement);

		// Set space
		void setSpace(float space);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Draw on top of element (used for drawing children over effects like marking)
        virtual void drawChildren() const;

    private:

        // Transform inner element
        void transformInnerElement();

        // Members
		FlowDirection mDirection;
        float mSpace;
        LerpValue mOffset;
    };
}

#endif // FLOW_H_
