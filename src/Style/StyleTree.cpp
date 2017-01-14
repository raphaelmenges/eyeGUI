//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleTree.h"

namespace eyegui
{
	StyleTree::StyleTree()
	{
		// Create root of tree with empty parent pointer
		mspRootNode = std::shared_ptr<StyleNode>(new StyleNode(STYLE_BASE_CLASS_NAME, std::shared_ptr<const StyleNode>()));
	}
	
	std::shared_ptr<const StyleValue<float> > StyleTree::fetchValue(std::string styleClass, StyleValue_float styleType) const
	{
		return std::shared_ptr<const StyleValue<float> >(); // TODO
	}

	std::shared_ptr<const StyleValue<glm::vec4> > StyleTree::fetchValue(std::string styleClass, StyleValue_vec4 styleType) const
	{
		return std::shared_ptr<const StyleValue<glm::vec4> >(); // TODO
	}

	void StyleTree::addStyleClass(std::string name, std::string parentName)
	{
		// Go through tree and search for parent class


		// Add style class to parent as new child node
	}

	template<typename T>
	void StyleTree::setValue(std::string styleClass, T styleType, std::string value)
	{
		// TODO
	}
}