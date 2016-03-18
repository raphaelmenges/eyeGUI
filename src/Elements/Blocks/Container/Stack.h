//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stack takes elements and displays them as a stack.

#ifndef STACK_H_
#define STACK_H_

#include "Container.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>
#include <memory>

namespace eyegui
{
    class Stack : public Container
    {
    public:

        enum class Alignment
        {
            FILL, CENTER, HEAD, TAIL
        };

        enum class RelativeScaling
        {
            BOTH_AXES, MAIN_AXIS
        };

        // Constructor
        Stack(
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
            RelativeScaling relativeScaling,
            Alignment alignment,
            float padding,
            float separator);

        // Destructor
        virtual ~Stack();

        // Attach elment
        void attachElement(std::unique_ptr<Element> upElement);

        // Tries to fetch next interactive element for selecting, returns NULL if fails
        virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

    protected:

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation
        virtual void specialTransformAndSize();

    private:

        // Members
        RelativeScaling mRelativeScaling;
        Alignment mAlignment;
        float mPadding; // [0..1]
        float mSeparator; // [0..1]
        RenderItem const * mpSeparatorItem;
        std::vector<glm::mat4> mSeparatorDrawMatrices;
    };
}

#endif // STACK_H_
