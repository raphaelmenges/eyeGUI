//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Virtual container that contains, updates and draws its children.

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
            bool dimming,
            bool adaptiveScaling,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            float innerBorder,
            bool showBackground);

        // Destructor
        virtual ~Container() = 0;

        // Activate or deactivate
        virtual void setActivity(bool active, bool fade);

        // Set marking
        virtual void setMarking(bool marking, int depth);

        // Get pointer to all children, recursively
        virtual std::set<Element*> getAllChildren() const;

        // Get ids of all children, recursively
        virtual std::set<std::string> getAllChildrensIds() const;

        // Replace an attached element, returns NULL if not found
        virtual std::unique_ptr<Element> replaceAttachedElement(
            Element* pTarget,
            std::unique_ptr<Element> upReplacement);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Reset filled by subclasses
        virtual void specialReset();

        // Draw on top of element (used for drawing children over effects like marking)
        virtual void drawChildren() const;

        // Members
        std::vector<std::unique_ptr<Element> > mChildren;

    private:

        // Members
        bool mShowBackground;
    };
}

#endif // CONTAINER_H_
