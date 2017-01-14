//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef STYLE_NODE_H_
#define STYLE_NODE_H_

#include "src/Style/StyleClass.h"

#include <vector>

namespace eyegui
{
	class StyleNode : public std::enable_shared_from_this<StyleNode> // enable shared pointer creation from this
	{
	public:

		// Constructor
		StyleNode(std::string name, std::weak_ptr<const StyleNode> wpParent)
		{
			// Save pointer to parent
			mwpParent = wpParent;

			// Try to get style class of parent to propagate values from there


			// Create encapsuled style class
			// mspStyleClass = std::shared_ptr<StyleClass>(new StyleClass(name, wpPa));

			// Get values from parent and propagate them to me

		}

		// Add child
		void addChild(std::string name)
		{
			mChildren.push_back(std::shared_ptr<StyleNode>(new StyleNode(name, shared_from_this())));
		}

		// Get node by name (this to bottom)
		//std::shared_ptr<StyleNode> fetchChild(std::string) const;

		// Search value in nodes (this to top)
		// TODO

		// Set value (automatically propagated)
		// TODO

	private:

		// Get contained style class
		std::weak_ptr<const StyleClass> getStyleClass() const { return mspStyleClass; }

		// Class contained by node
		std::shared_ptr<StyleClass> mspStyleClass;

		// Pointer to parent
		std::weak_ptr<const StyleNode> mwpParent; // empty for root

												  // Children
		std::vector<std::shared_ptr<StyleNode> > mChildren;
	};
}

#endif // STYLE_NODE_H_