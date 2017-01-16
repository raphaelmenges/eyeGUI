//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "StyleTree.h"

#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{
	StyleTree::StyleTree()
	{
		// Create root of tree with empty parent pointer and base class
		mspRoot = StyleClassBuilder().construct(STYLE_BASE_CLASS_NAME, StyleClassConstructionType::BASE_CLASS);
	}

	std::shared_ptr<const StyleClass> StyleTree::addStyleClass(std::string name, std::string parentName)
	{
		// Check, whether class does already exist
		if (auto spStyleClass = mspRoot->fetchThisOrChild(name))
		{
			// Does already exist
			throwWarning(OperationNotifier::Operation::PARSING, "Style class already exists and value is set there: " + name);
			return spStyleClass;
		}

		// Go through tree and search for parent class
		auto spParent = mspRoot->fetchThisOrChild(parentName);
		if (!spParent)
		{
			// Suggested parent not found
			throwWarning(OperationNotifier::Operation::PARSING, "Suggested parent of style class not found. Using root: " + name);
			spParent = mspRoot;
		}
		return spParent->addChild(name);
	}
	
	std::shared_ptr<const StyleClass> StyleTree::fetchStyleClass(std::string name) const
	{
		auto spStyleClass = mspRoot->fetchThisOrChild(name);
		if (!spStyleClass) { spStyleClass = mspRoot; };
		return std::move(spStyleClass);
	}
}