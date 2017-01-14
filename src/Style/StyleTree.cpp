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
		// Create root of tree with empty parent pointer, so weak pointer is invalid after constructor call
		mspRoot = std::shared_ptr<StyleClass>(new StyleClass(STYLE_BASE_CLASS_NAME, std::shared_ptr<const StyleClass>()));

		// Fill root with base / inital values
		mspRoot->fillWithBaseValues();
	}

	std::shared_ptr<const StyleClass> StyleTree::addStyleClass(std::string name, std::string parentName)
	{
		// Check, whether class does already exist
		if (auto spStyleClass = mspRoot->fetchThisOrChild(name))
		{
			// TODO: warning, that class exists, maybe under different parent
			return spStyleClass;
		}

		// Go through tree and search for parent class
		auto spParent = mspRoot->fetchThisOrChild(parentName);
		if (!spParent)
		{
			// TODO: warning, about attachment to root
			spParent = mspRoot;
		}
		return spParent->addChild(name);
	}
	
	std::shared_ptr<const StyleClass> StyleTree::fetchStyleClass(std::string name) const
	{
		return mspRoot->fetchThisOrChild(name);
	}
}