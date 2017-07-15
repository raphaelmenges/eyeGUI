//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract base class of all elements. Enumeration used for later identification
// of pointers to specialized variants. Has vector holding unique pointer to
// child elements but this construct is only used when necessary, for example
// in stack, grid or drop button. Elements have an unique id inside the layout
// they belong to. Style classes are saved as pointers. Element can have only
// one parent element which is set to null for the root element of the frame.
// Transformation and size is calculated using a mechanism first asking the
// children with "evaluateSize" how much of the available space they would use
// and then telling all the children their size and transformation.

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "include/eyeGUI.h"
#include "src/Object.h"
#include "src/Styleable.h"
#include "src/Rendering/AssetManager.h"
#include "src/Style/StyleClass.h"
#include "src/Utilities/LerpValue.h"
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
    class ElementFactory;

    class Element : public Object, public Styleable
    {
    public:

        // Enumeration of elements
        enum class Type {
            ELEMENT,
            PICTURE,
            BLANK,
            NOTIFIER_ELEMENT,
            INTERACTIVE_ELEMENT,
            ICON_ELEMENT,
            SENSOR,
            BUTTON,
            CIRCLE_BUTTON,
            BOX_BUTTON,
            DROP_BUTTON,
            CONTAINER,
            BLOCK,
            STACK,
            GRID,
            TEXT_BLOCK,
            KEYBOARD,
            WORD_SUGGEST,
            FLOW,
            PROGRESS_BAR,
			TEXT_EDIT,

			// Experimental
			FUTURE_KEYBOARD
        };

        // Enumeration of rendering masks
        enum class RenderingMask {
            BOX, CIRCLE
        };

        // Orientation of element
        enum class Orientation { HORIZONTAL, VERTICAL };

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

        // Type string getter
        std::string getTypeString() const;

        // Id getter
        std::string getId() const;

        // Parent
        Element* getParent() const;

        // Activity
        bool isActive() const;

        // Orientation
        Orientation getOrientation() const;

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

		// Flash
		void flash();

        // Set marking
        virtual void setMarking(bool marking, int depth);

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
        virtual std::set<Element*> getAllChildren() const;

        // Get ids of all children, recursively
        virtual std::set<std::string> getAllChildrenIds() const;

        // Get all ids, of children and this node
        std::set<std::string> getAllIds() const;

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

        // Check for any parent with certain type
        bool checkForParentType(Element::Type type) const;

		// Parse and set value of style property owned by element
		template<typename Type>
		void parseElementStylePropertyValue(Type styleType, std::string value)
		{
			mspStyleClass->parseValue(styleType, value);
		}

		// Update keyboard layouts in element tree (TODO: would be nicer to have general execute on tree function)
		virtual void updateKeyboardLayout();

    protected:

		// Factory is friend
        friend class ElementFactory;

		// Protected constructor
		Element(
			std::string id,
			std::vector<std::string> styles,
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

        // Draw on top of element
        virtual void drawChildren() const;

        // Getter
        float getDim() const;
        float getMultipliedDimmedAlpha() const;

		// Get raw values of styling property
		template<typename Type>
		typename style::PropertyInfo<Type>::type getStyleValue(Type type) const
		{
			// Go over the property in the differenct classes
			std::shared_ptr<const StyleProperty<typename style::PropertyInfo<Type>::type> > spStyleProperty;

			// Check own class for some set value
			spStyleProperty = mspStyleClass->fetchProperty(type);
			if (spStyleProperty->isSet()) // if the value of this property was actively set, use it!
			{
				return spStyleProperty->get();
			}

			// Go over assigned classes of global style tree
			for (const auto& rspStyleClass : mStyleTreeClasses)
			{
				// Check whether property is really set or just base
				spStyleProperty = rspStyleClass->fetchProperty(type);
				if (spStyleProperty->isSet()) // no base, use this property's value
				{
					break;
				}
				else // just base, try next class
				{
					continue;
				}
			}
			return spStyleProperty->get();
		}

		// Getter of individual style class owned by element
		std::shared_ptr<StyleClass> fetchElementStyleClass() const;

        // Notify about interaction with element
        void notifyInteraction(std::string interactionType, std::string interactionInfoA = "") const;

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
        bool mAdaptiveScaling;
        LerpValue mAdaptiveScale; // [0..1]
        bool mForceUndim; // At the moment only used by drop button
                          // to be undimmed while showing inner elements

    private:

        // Members
        std::string mId;
        Element* mpParent; // NULL for root
        float mBorder; // [0..1]
        Orientation mOrientation;
        std::unique_ptr<Element> mupReplacedElement;
        bool mHidden;
        RenderingMask mRenderingMask;
        RenderItem const * mpActivityItem;
        RenderItem const * mpDimItem;
		RenderItem const * mpFlashItem;
        RenderItem const * mpMarkItem;
        bool mPenetratedLastUpdate; // useful for interaction notification
        glm::vec2 mLastGazeCoordinate; // useful for interaction notification

        bool mActive;
        LerpValue mActivity; // [0..1]

        bool mDimming;
        LerpValue mDim; // [0..1]

		LerpValue mFlash; // [0..1]. Set to 1 when should flash and goes back to zero

        bool mMarking;
        LerpValue mMark; // [0..1]

		std::shared_ptr<StyleClass> mspStyleClass; // individual style class owned by element

    };

    namespace element_helper
    {
        // Parsing value for style tree
        template<std::size_t I = 0, typename... Tp>
        inline typename std::enable_if<I == sizeof...(Tp), void>::type // base case of I == number of tuple elements
            internalParse(const std::tuple<Tp...>& t, Element* pElement, std::string styleType, std::string value)
        {} // if comes to here, no string was matched...

        template<std::size_t I = 0, typename... Tp>
        inline typename std::enable_if < I < sizeof...(Tp), void>::type
            internalParse(const std::tuple<Tp...>& t, Element* pElement, std::string styleType, std::string value)
        {
            // Fetch map from tuple
            const auto& rMap = std::get<I>(t);
            auto iterator = rMap.find(styleType);
            if (iterator != rMap.end()) { pElement->parseElementStylePropertyValue(iterator->second, value); return; }

            // String not yet found in any map, proceed
            internalParse<I + 1, Tp...>(t, pElement, styleType, value); // recursion call
        }

        // Call to parse style type value where both type and value are encoded as string
        void parse(Element* pElement, std::string styleType, std::string value);
    }
}

#endif // ELEMENT_H_
