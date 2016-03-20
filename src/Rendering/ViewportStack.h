//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stack of intersecting rectangular viewports to mimic simple stencil test.
// Used for example for keyboard to render keys only inside of keyboard element.
// Implemented as singleton structure to be accessible from all classes.

#ifndef VIEWPORT_STACK_H_
#define VIEWPORT_STACK_H_

#include "src/Utilities/OperationNotifier.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"
#include <stack>

// Defines for easier use
#define pushViewport eyegui::ViewportStack::push
#define popViewport eyegui::ViewportStack::pop
#define setWindowSizeInViewportStack eyegui::ViewportStack::setWindowSize

namespace eyegui
{
	class ViewportStack
	{
	public:

		// Push viewport on stack
		static void push(int x, int y, int width, int height)
		{
			getInstance()->internalPush(x, y, width, height);
		}

		// Pop viewport from stack
		static void pop()
		{
			getInstance()->internalPop();
		}

		// Set window size (should be called by GUI)
		static void setWindowSize(int width, int height)
		{
			getInstance()->internalSetWindowSize(width, height);
		}

	private:

		// Struct for rect (origin in upper left corner)
		struct Rect
		{
			int x;
			int y;
			int width;
			int height;
		};

		// Private constructor
		ViewportStack()
		{
			mWindowWidth = 0;
			mWindowHeight = 0;
		}

		// Destructor
		virtual ~ViewportStack() {}

		// Private copy constuctor
		ViewportStack(ViewportStack const&) {}

		// Push viewport on stack
		void internalPush(int x, int y, int width, int height)
		{
			mRects.push(Rect());
		}

		// Pop viewport from stack
		void internalPop()
		{
			if (!(mRects.empty()))
			{
				mRects.pop();
			}
			else
			{
				throwError(OperationNotifier::Operation::BUG, "Tried to do pop on empty viewport stack");
			}
			
		}

		// Set window size
		void internalSetWindowSize(int width, int height)
		{
			mWindowWidth = width;
			mWindowHeight = height;
		}

		// Check for instance existence
		static ViewportStack* getInstance()
		{
			if (pInstance == NULL)
			{
				static ViewportStack viewportStack;
				pInstance = &viewportStack;
			}
			return pInstance;
		}

		// The pointer to the single instance
		static ViewportStack* pInstance;

		// Members
		std::stack<Rect> mRects;
		int mWindowWidth;
		int mWindowHeight;
	};
}

#endif // VIEWPORT_STACK_H_