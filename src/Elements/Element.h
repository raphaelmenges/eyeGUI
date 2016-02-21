//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract base class of all elements. Enumeration used for later identification
// of pointers to specialized variants. Has vector holding unique pointer to
// child elements but this construct is only used when necessary, for example
// in stack, grid or drop button. Elements have an unique id inside the layout
// they belong to. Style is saved as string and set do a default value by parser
// when none is defined and the parent has no set style. Element can have only
// one parent element which is set to null for the root element of the frame.
// Transformation and size is calculated using a mechanism first asking the
// children with "evaluateSize" how much of the available space they would use
// and then telling all the children their size and transformation. Since drop
// button has a child element which is updated and drawn by the frame, child
// elements are not updated and drawn automatically by element class.

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "include/eyeGUI.h"
#include "Object.h"
#include "Rendering/AssetManager.h"
#include "Style.h"
#include "LerpValue.h"
#include "externals/GLM/glm/glm.hpp"

#include <memory>
#include <vector>
#include <set>

namespace eyegui
{
    // Forward declaration
    class Layout;
    class NotificationQueue;
    class Frame;
    class InteractiveElement;

    class Element : public Object
    {
    public:

        // Enumeration of elements
        enum class Type {
            ELEMENT,
            PICTURE,
            BLANK,
            NOTIFIER_ELEMENT,
            INTERACTIVE_ELEMENT,
			ICON_INTERACTIVE_ELEMENT,
            SENSOR,
            BUTTON,
            CIRCLE_BUTTON,
            BOX_BUTTON,
            DROP_BUTTON,
            BLOCK,
            STACK,
            GRID,
            TEXT_BLOCK,
            KEYBOARD
        };

        // Orientation of element
        enum class Orientation { HORIZONTAL, VERTICAL };

        // Constructor
        Element(
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
            bool adaptiveScaling);

        // Destructor
        virtual ~Element() = 0;

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

        // Type getter
        Type getType() const;

        // Id getter
        std::string getId() const;

        // Parent
        Element* getParent() const;

        // Activity
        bool isActive() const;

        // Orientation
        Orientation getOrientation() const;

        // Getter for style
        Style const * getStyle() const;

        // Setter for style
        void setStyle(std::string styleName);

        // Getter for name of style
        std::string getStyleName() const;

        // Setter for alpha
        void setAlpha(float alpha);

        // Getter for alpha
        float getAlpha() const;

        // Activate or deactivate
        virtual void setActivity(bool active, bool fade);

        // Setter for dimming
        void setDimming(bool dimming);

        // Getter for dimming
        bool isDimming() const;

        // Set marking
        void setMarking(bool marking, int depth);

        // Getter for marking
        bool isMarking() const;

        // Get layout
        Layout const * getLayout() const;

        // Get frame
        Frame* getFrame() const;

        // Get asset manager
        AssetManager* getAssetManager() const;

        // Get notification queue
        NotificationQueue* getNotificationQueue() const;

        // Get border
        float getBorder() const;

        // Get pointer to all children, recursively
        std::set<Element*> getAllChildren() const;

        // Get ids of all children, recursively
        std::set<std::string> getAllChildrensIds() const;

        // Change transformation and size (pixel values)
        void transformAndSize(int x, int y, int width, int height);

        // Get relative and adaptive scale combined to dynamic scale
        float getDynamicScale() const;

        // Get relative scale
        float getRelativeScale() const;

        // Get adaptive scaling
        bool getAdaptiveScaling() const;

        // Set hiding
        void setHiding(bool hidden);

        // Updating, returns adaptive scale
        float update(float tpf, float alpha, Input* pInput, float dim);

        // Drawing
        void draw() const;

        // Resetting
        void reset();

        // Check before transformation, how much space is needed
        virtual void evaluateSize(
            int availableWidth,
            int availableHeight,
            int& rWidth,
            int& rHeight) const;

        // Starts the getting of the next interactive element, returns NULL if no available
        virtual InteractiveElement* nextInteractiveElement();

        // Tries to fetch next interactive element for selecting, returns NULL if fails
        virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

        // Replace an attached element, returns NULL if not found
        virtual std::unique_ptr<Element> replaceAttachedElement(
            Element* pTarget,
            std::unique_ptr<Element> upReplacement);

        // Commit replaced element to this element
        void commitReplacedElement(std::unique_ptr<Element> upElement, bool fade);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput) = 0;

        // Drawing filled by subclasses
        virtual void specialDraw() const = 0;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize() = 0;

        // Reset filled by subclasses
        virtual void specialReset() = 0;

        // Implemented by subclasses
        virtual bool mayConsumeInput() = 0;

        // Checks, whether element is penetrated by input
        virtual bool penetratedByInput(Input const * pInput) const;

        // Members
        int mX, mY, mWidth, mHeight; // ONLY PIXEL BASED VALUES HERE
        Type mType;
        Layout const * mpLayout;
        Frame* mpFrame;
        AssetManager* mpAssetManager;
        NotificationQueue* mpNotificationQueue;
        float mRelativeScale; // [0..]
        float mAlpha; // [0..1]
        float mBorderAspectRatio;
        glm::mat4 mFullDrawMatrix;
        LerpValue mActivity; // [0..1]
        bool mDimming;
        LerpValue mDim; // [0..1] One means full dimming
        bool mForceUndim; // At the moment only used by drop button
                          // to be undimmed while showing inner elements
        bool mActive;
        bool mAdaptiveScaling;
        LerpValue mAdaptiveScale; // [0..1]
        bool mMarking;
        LerpValue mMark;

        // This vector is the owner of all children. May be empty!
        std::vector<std::unique_ptr<Element> > mChildren;

    private:

        // Members
        std::string mId;
        Element* mpParent;
        float mBorder; // [0..1]
        Orientation mOrientation;
        std::string mStyleName;
        Style const * mpStyle;
        std::unique_ptr<Element> mupReplacedElement;
        bool mHidden;
    };
}

#endif // ELEMENT_H_
