//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef STYLE_TREE_H_
#define STYLE_TREE_H_

#include "src/Style/StyleClass.h"
#include "src/Style/StyleNode.h"

namespace eyegui
{
	class StyleTree
	{
	public:

		// Constructor
		StyleTree();

		// Get style (searches recursively in tree)
		std::shared_ptr<const StyleValue<float> > fetchValue(std::string styleClass, StyleValue_float styleType) const;
		std::shared_ptr<const StyleValue<glm::vec4> > fetchValue(std::string styleClass, StyleValue_vec4 styleType) const;

		// Add class
		void addStyleClass(std::string name, std::string parentName);

		// Set value (class must be added before calling this)
		template<typename T>
		void setValue(std::string styleClass, T styleType, std::string value);

	private:

		// Root node of style tree
		std::shared_ptr<StyleNode> mspRootNode;
	};
}

#endif // STYLE_TREE_H_