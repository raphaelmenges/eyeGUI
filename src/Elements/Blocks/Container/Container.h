//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Virtual container that updates and draws its children.

#ifndef CONTAINER_H_
#define CONTAINER_H_

#include "Elements/Blocks/Block.h"

namespace eyegui
{
    class Container : public Block
    {
    public:

        // Constructor
        Container(
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
            bool consumeInput,
            float innerBorder,
            bool showBackground);

        // Destructor
        virtual ~Container() = 0;

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Members
        bool mShowBackground;
    };
}

#endif // CONTAINER_H_
