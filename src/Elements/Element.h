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
// one parent element which is set to null for the root element of the layout.
// Transformation and size is calculated using a mechanism first asking the
// children with "evaluateSize" how much of the available space they would use
// and then telling all the children their size and transformation.

#ifndef ELEMENT_H_
#define ELEMENT_H_

#include "eyeGUI.h"
#include "Rendering/AssetManager.h"
#include "Input.h"
#include "Style.h"
#include "External/GLM/glm/glm.hpp"

#include <memory>
#include <vector>
#include <set>

namespace eyegui
{
	// Forward declaration
	class Layout;
	class InteractiveElement;

	class Element
	{
	public:

		// Enumeration of elements
		enum class Type {
			ELEMENT,
			PICTURE,
			BLANK,
			INTERACTIVE_ELEMENT,
			SENSOR,
			BUTTON,
			CIRCLE_BUTTON,
			BOX_BUTTON,
			DROP_BUTTON,
			BLOCK,
			STACK,
			GRID
		};

		// Orientation of element
		enum class Orientation { HORIZONTAL, VERTICAL };

		// Constructor
		Element(
			std::string id,
			std::string styleName,
			Element* pParent,
			Layout* pLayout,
			AssetManager* pAssetManager,
			float relativeScale,
			float border);

		// Destructor
		virtual ~Element() = 0;

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

		// Style
		Style const * getStyle() const;

		// Name of style
		std::string getStyleName() const;

		// Setter for alpha
		void setAlpha(float alpha);

		// Getter for alpha
		float getAlpha() const;

		// Activate or deactivate
		virtual void setActivity(bool active, bool setImmediately);

		// Get layout
		Layout* getLayout() const;

		// Get asset manager
		AssetManager* getAssetManager() const;

		// Get border
		float getBorder() const;

		// Get pointer to all children, recursively
		std::set<Element *> getAllChildren();

		// Get ids of all children, recursively
		std::set<std::string> getAllChildrensIds() const;

		// Change transformation and size (pixel values)
		void transformAndSize(int x, int y, int width, int height);

		/*
		__________> x
		|
		|
		|
		|
		v
		y

		x coordinate = minimal left value in pixels
		y coordinate = minimal top value in pixels

		*/

		// Transformation and size
		int getX() const;
		int getY() const;
		int getWidth() const;
		int getHeight() const;
		float getRelativeScale() const;

		// Updating
		void update(float tpf, float alpha, Input* pInput);

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
		void commitReplacedElement(std::unique_ptr<Element> upElement, bool doFading);

	protected:

		// Updating filled by subclasses
		virtual void specialUpdate(float tpf, Input* pInput) = 0;

		// Drawing filled by subclasses
		virtual void specialDraw() const = 0;

		// Transformation filled by subclasses
		virtual void specialTransformAndSize() = 0;

		// Reset filld by subclasses
		virtual void specialReset() = 0;

		// Convert pixel space to drawing space
		glm::mat4 calculateDrawMatrix(int x, int y, int width, int height) const;

		// Members
		Type mType;
		Layout* mpLayout;
		AssetManager* mpAssetManager;
		int mX, mY, mWidth, mHeight; // ONLY PIXEL BASED VALUES HERE
		float mRelativeScale; // [0..]
		float mAlpha; // [0..1]
		float mBorderAspectRatio;
		glm::mat4 mDrawMatrix;
		float mActivity; // [0..1]
		bool mActive;

		// This vector is the owner of all possible children. May be empty!
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
	};
}

#endif // ELEMENT_H_
