//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stack of intersecting rectangular scissors to mimic simple stencil test.
// Used for example for keyboard to render keys only inside of keyboard element.
// Implemented as singleton structure to be accessible from all classes.

#ifndef SCISSOR_STACK_H_
#define SCISSOR_STACK_H_

#include "src/Utilities/OperationNotifier.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

// Defines for easier use
#define pushScissor eyegui::ScissorStack::push
#define popScissor eyegui::ScissorStack::pop
#define initScissorStack eyegui::ScissorStack::initForDrawing

namespace eyegui
{
	class ScissorStack
	{
	public:

		// Push scissor on stack
		static void push(int x, int y, int width, int height)
		{
			getInstance()->internalPush(x, y, width, height);
		}

		// Pop scissor from stack
		static void pop()
		{
			getInstance()->internalPop();
		}

		// Init for drawing (should be called only by GUI)
		static void initForDrawing(int width, int height)
		{
			getInstance()->internalInitForDrawing(width, height);
		}

	private:

		// Struct for rect (origin in upper left corner)
		struct Rect
		{
			Rect(int x, int y, int width, int height)
			{
				this->x = x;
				this->y = y;
				this->width = width;
				this->height = height;
			}

			int x;
			int y;
			int width;
			int height;
		};

		// Private constructor
		ScissorStack()
		{
			mWindowWidth = 0;
			mWindowHeight = 0;
		}

		// Destructor
		virtual ~ScissorStack() {}

		// Private copy constuctor
		ScissorStack(ScissorStack const&) {}

		// Push scissor on stack
		void internalPush(int x, int y, int width, int height)
		{
			mRects.push_back(Rect(x, y, width, height));
			scissor();
		}

		// Pop scissor from stack
		void internalPop()
		{
			if (!(mRects.empty()))
			{
				mRects.pop_back();
				scissor();
			}
			else
			{
				throwError(OperationNotifier::Operation::BUG, "Tried to do pop from an empty viewport stack");
			}	
		}

		// Set window size
		void internalInitForDrawing(int width, int height)
		{
			mWindowWidth = width;
			mWindowHeight = height;

			// Clear stack
			mRects.clear();

			// Scissor
			scissor();
		}

		// Do scissoring
		void scissor() const
		{
			// Go over stack and intersect in space with origin in upper left corner
			int x1 = 0;
			int x2 = mWindowWidth;
			int y1 = 0;
			int y2 = mWindowHeight;
			int a1, a2, b1, b2;
			for (const Rect& rRect : mRects)
			{
				// Read values from rect
				a1 = rRect.x;
				a2 = rRect.x + rRect.width;
				b1 = rRect.y;
				b2 = rRect.y + rRect.height;
				
				// Intersection
				x1 = glm::max(x1, a1);
				x2 = glm::min(x2, a2);
				y1 = glm::max(y1, b1);
				y2 = glm::min(y2, b2);
				if (x1 > x2) { x2 = x1; }
				if (y1 > y2) { y2 = y1; }	
			}

			// Transform to OpenGL space
			glScissor(x1, mWindowHeight - y2, x2 - x1, y2 - y1);
		}

		// Check for instance existence
		static ScissorStack* getInstance()
		{
			if (pInstance == NULL)
			{
				static ScissorStack scissorStack;
				pInstance = &scissorStack;
			}
			return pInstance;
		}

		// The pointer to the single instance
		static ScissorStack* pInstance;

		// Members
		std::vector<Rect> mRects; // stack implemented using vector
		int mWindowWidth;
		int mWindowHeight;
	};
}

#endif // SCISSOR_STACK_H_