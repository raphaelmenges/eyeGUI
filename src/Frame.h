//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Frame is owner of elements. Elements can register child elements for
// updating and rendering as front elements, which are then updated before
// the other elements and drawn on top by the frame.

#ifndef FRAME_H_
#define FRAME_H_

#include "include/eyeGUI.h"
#include "Object.h"
#include "Elements/Elements.h"
#include "LerpValue.h"

#include <memory>
#include <map>
#include <set>

namespace eyegui
{
    // Forward declaration
    class Layout;

    class Frame : public Object
    {
    public:

        // Constructor
        Frame(
            Layout * const pLayout,
            float relativePositionX,
            float relativePositionY,
            float relativeSizeX,
            float relativeSizeY);

        // Destuctor
        virtual ~Frame();

        // Transformation and size in pixels
        virtual int getX() const;
        virtual int getY() const;
        virtual int getWidth() const;
        virtual int getHeight() const;

        // Get relative screen position and size
        virtual float getRelativePositionOnLayoutX() const;
        virtual float getRelativePositionOnLayoutY() const;
        virtual float getRelativeSizeOnLayoutX() const;
        virtual float getRelativeSizeOnLayoutY() const;

        // Updating
        void update(float tpf, float alpha, Input* pInput);

        // Drawing
        void draw() const;

        // Make resize necessary
        void makeResizeNecessary();

        // Attach root
        void attachRoot(std::unique_ptr<Element> upElement);

        // Remove root
        std::unique_ptr<Element> replaceRoot(std::unique_ptr<Element> upElement);

        // Register front element for early update and late drawing
        void registerFrontElementForUpdateAndDraw(Element* pElement, bool visible);

        // Set alpha value of front element
        void setFrontElementAlpha(Element* pElement, float alpha);

        // Commit dying replaced element
        void commitDyingReplacedElement(std::unique_ptr<Element> upElement);

        // Set visibility
        void setVisibility(bool visible, bool fade);

        // Get first interactive element. Returns null, if none found
        InteractiveElement* getFirstInteractiveElement() const;

        // Reset elements
        void resetElements();

        // Remove front elements of a element
        void removeFrontElementsOfElement(Element* pTarget);

        // Get pointer to all elements, recursively
        std::set<Element*> getAllElements() const;

        // Get all elements' ids
        std::set<std::string> getAllElementsIds() const;

        // Set removed fading alpha
        void setRemovedFadingAlpha(float alpha);

        // Get removed fading alpha
        float getRemovedFadingAlpha() const;

        // Set removed
        void setRemoved();

        // Is removed?
        bool isRemoved() const;

        // Translation
        void translate(float translateX, float translateY);

        // Scaling
        void scale(float scaleX, float scaleY);

        // Set position
        void setPosition(float relativePositionX, float relativePositionY);

        // Set size
        void setSize(float relativeSizeX, float relativeSizeY);

    private:

        // Resize function
        void internalResizing();

        // Clamp size
        void clampSize();

        // Members
        Layout const * mpLayout;
        std::unique_ptr<Element> mupRoot;
        LerpValue mFrameAlpha;
        float mCombinedAlpha;
        bool mVisible;
        std::vector<Element*> mFrontElements;
        std::map<Element*, float> mFrontElementAlphas;
        std::vector<std::unique_ptr<Element> > mDyingReplacedElements;
        bool mResizeNecessary;
        float mRelativePositionX;
        float mRelativePositionY;
        float mRelativeSizeX;
        float mRelativeSizeY;
        bool mRemoved;
        float mRemovedFadingAlpha;
    };
}


#endif // FRAME_H_
