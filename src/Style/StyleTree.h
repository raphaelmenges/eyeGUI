//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Tree of style classes with single root. Guarantees that name of style
// classes is unique. Returns only const pointers to style classes,
// for value setting the corresponding method must be called. GUI objects
// has one tree which is filled by loaded stylesheets. Elements and layouts
// are assigned classes, whereby all properties directly set in element class
// do override the values from this tree.

#ifndef STYLE_TREE_H_
#define STYLE_TREE_H_

#include "src/Style/StyleClass.h"
#include "src/Style/StyleDefinitions.h"

namespace eyegui
{
	class StyleTree
	{
	public:

		// Constructor
		StyleTree();

		// Add style class class
		std::shared_ptr<const StyleClass> addStyleClass(std::string name, std::string parentName = STYLE_BASE_CLASS_NAME);

		// Fetch style class
		std::shared_ptr<const StyleClass> fetchStyleClass(std::string name) const;

		// Parse and set value by name of style class
		template<typename Type>
		void parseValue(std::string styleClass, Type styleType, std::string value)
		{
			// Fetch style class
			if (auto spStyleClass = mspRoot->fetchThisOrChild(styleClass))
			{
				spStyleClass->parseValue(styleType, value);
			}
		}

	private:

		// Root node of style tree, contains base style class
		std::shared_ptr<StyleClass> mspRoot;
	};

	namespace style_tree_helper
	{
		// Parsing value for style tree
		template<std::size_t I = 0, typename... Tp>
		inline typename std::enable_if<I == sizeof...(Tp), void>::type // base case of I == number of tuple elements
			internalParse(const std::tuple<Tp...>& t, std::shared_ptr<StyleTree> spStyleTree, std::string styleClass, std::string styleType, std::string value)
		{}

		template<std::size_t I = 0, typename... Tp>
		inline typename std::enable_if < I < sizeof...(Tp), void>::type
			internalParse(const std::tuple<Tp...>& t, std::shared_ptr<StyleTree> spStyleTree, std::string styleClass, std::string styleType, std::string value)
		{
			// Fetch map from tuple
			const auto& rMap = std::get<I>(t);
			auto iterator = rMap.find(styleType);
			if (iterator != rMap.end()) { spStyleTree->parseValue(styleClass, iterator->second, value); return; }

			// String not yet found in any map, proceed
			internalParse<I + 1, Tp...>(t, spStyleTree, styleClass, styleType, value); // recursion call
		}

		// Call to parse style type value where both type and value are encoded as string
		void parse(std::shared_ptr<StyleTree> spStyleTree, std::string styleClass, std::string styleType, std::string value);		
	}
}

#endif // STYLE_TREE_H_
