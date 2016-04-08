//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Container.h"

namespace eyegui
{
    Container::Container(
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
        bool showBackground) : Block(
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
            adaptiveScaling,
            consumeInput,
            backgroundFilepath,
            backgroundAlignment,
            innerBorder)
    {
        // Fill members
        mShowBackground = showBackground;
    }

    Container::~Container()
    {
        // Nothing to do
    }

    void Container::setActivity(bool active, bool fade)
    {
        Element::setActivity(active, fade);

        // Do it for children
        // Do it for all children
        for (const std::unique_ptr<Element>& rElement : mChildren)
        {
            rElement->setActivity(active, fade);
        }
    }

    void Container::setMarking(bool marking, int depth)
    {
        Element::setMarking(marking, depth);

        // Affect children
        if (depth != 0)
        {
            // Only decrement positive values
            if (depth > 0)
            {
                depth--;
            }

            // Tell children about it
            for (const std::unique_ptr<Element>& rupElement : mChildren)
            {
                rupElement->setMarking(marking, depth);
            }
        }
    }

    std::set<Element*> Container::getAllChildren() const
    {
        std::set<Element*> elements;

        // Go over children and collect pointers
        for (const std::unique_ptr<Element>& rupChild : mChildren)
        {
            std::set<Element*> childrensElements = rupChild->getAllChildren();
            elements.insert(childrensElements.begin(), childrensElements.end());
            elements.insert(rupChild.get());
        }

        return elements;
    }

    std::set<std::string> Container::getAllChildrensIds() const
    {
        std::set<std::string> ids;

        // Go over children and collect ids
        for (const std::unique_ptr<Element>& rupChild : mChildren)
        {
            // Insert id of children of child
            std::set<std::string> childrensIds = rupChild->getAllChildrensIds();
            ids.insert(childrensIds.begin(), childrensIds.end());

            // Only insert id of child if there is one
            std::string id = rupChild->getId();
            if (id != "")
            {
                ids.insert(rupChild->getId());
            }
        }

        return ids;
    }

    std::unique_ptr<Element> Container::replaceAttachedElement(
        Element* pTarget,
        std::unique_ptr<Element> upReplacement)
    {
        // Search in children for element to replace
        int index = -1;
        int i = 0;
        for (std::unique_ptr<Element>& rupElement : mChildren)
        {
            if (rupElement.get() == pTarget)
            {
                index = i;
                break;
            }
            i++;
        }

        // Child found, so replace it
        if (index > -1)
        {
            // Replace it
            std::unique_ptr<Element> upTarget = std::move(mChildren[i]);
            mChildren[i] = std::move(upReplacement);
            return std::move(upTarget);
        }

        // Fallback if not found
        return NULL;
    }

    float Container::specialUpdate(float tpf, Input* pInput)
    {
        float maxAdaptiveScaleOfChildren = 0;

        // Update the elements
        for (std::unique_ptr<Element>& rupElement : mChildren)
        {
            float childAdaptiveScale = rupElement->update(tpf, mAlpha, pInput, getDim());
            maxAdaptiveScaleOfChildren = std::max(maxAdaptiveScaleOfChildren, childAdaptiveScale);
        }

        // Super call after children (may consume input first)
        float adaptiveScale = Block::specialUpdate(tpf, pInput);

        // Return adaptive scale
        return std::max(adaptiveScale, maxAdaptiveScaleOfChildren);
    }

    void Container::specialDraw() const
    {
        // Only draw block in background if allowed
        if(mShowBackground)
        {
            Block::specialDraw();
        }
    }

    void Container::specialReset()
    {
        Block::specialReset();

        // Go over chilren and reset
        for (const std::unique_ptr<Element>& rupElement : mChildren)
        {
            rupElement->reset();
        }
    }

    void Container::drawChildren() const
    {
        // Draw children
        for (const std::unique_ptr<Element>& rupElement : mChildren)
        {
            rupElement->draw();
        }
    }
}
