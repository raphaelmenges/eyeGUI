//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Object is abstract superclass for Frame and Element.

#ifndef OBJECT_H_
#define OBJECT_H_

namespace eyegui
{
	class Object
	{
	public:

		// Constructor
		Object();

		// Destructor
		virtual ~Object() = 0;

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

		// Transformation and size in pixels
		virtual int getX() const = 0;
		virtual int getY() const = 0;
		virtual int getWidth() const = 0;
		virtual int getHeight() const = 0;

		// Get relative screen position and size (implemented by subclasses only)
		virtual float getRelativePositionOnLayoutX() const = 0;
		virtual float getRelativePositionOnLayoutY() const = 0;
		virtual float getRelativeSizeOnLayoutX() const = 0;
		virtual float getRelativeSizeOnLayoutY() const = 0;
	};
}

#endif