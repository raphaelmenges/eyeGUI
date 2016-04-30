//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Layer with layouts.

#ifndef LAYER_H_
#define LAYER_H_

#include "Layout.h"

namespace eyegui
{
	class Layer
	{
	public:

		// Constructor
		Layer();

		// Destructor
		virtual ~Layer();

		// Updating
		void update(float tpf, Input* pInput);

		// Drawing
		void draw() const;

		// Resize
		void makeResizeNecessary(bool force);

		// Add layout
		void addLayout(std::unique_ptr<Layout> upLayout);

		// Remove layout, returns success
		bool removeLayout(Layout const * pLayout);

		// Find index of layout, returns -1 if fails
		int findLayout(Layout const * pLayout) const;

		// Move layout
		void moveLayout(int oldIndex, int newIndex);

		// Getter for layout count
		int getLayoutCount() const;

	protected:

		// Members
		std::vector<std::unique_ptr<Layout> > mLayouts;
	};
}

#endif // LAYER_H_