//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Definitions for the style system. There is following structure:
//   StyleClass: holds one map for each property class
//   ProtertyClass: is defined by "type" of property, like duration
//   PropertyValue: raw type is defined for each property class, like float
//   Property: is instance of property class and defind by one value, for example AnimationDuration

#ifndef STYLE_DEFINITIONS
#define STYLE_DEFINITIONS

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"

#include "externals/GLM/glm/glm.hpp"

#include <map>
#include <tuple>

namespace eyegui
{
	namespace style
	{
		// TODO
		// - template specialization providing constraint per property class
		// - template specialization providing parse function per property class

		// Compile time mapping of property class to property value
		template<typename Type> struct PropertyValue;
		template<> struct PropertyValue<StylePropertyFloat>		{ typedef float type; };
		template<> struct PropertyValue<StylePropertyVec4>		{ typedef glm::vec4 type; };
		template<> struct PropertyValue<StylePropertyString>	{ typedef std::string type; };

		// Typedef of tuple containing maps of property classes for style class
		template<typename Type> using PropertyMap = std::map<Type, std::shared_ptr<StyleProperty<typename PropertyValue<Type>::type> > >;
		typedef
			std::tuple<
			PropertyMap<StylePropertyFloat>,
			PropertyMap<StylePropertyVec4>,
			PropertyMap<StylePropertyString>
			> PropertyMaps;

		// Indices of property classes in tuple above (easier to write it manually instead of complex recursive template structure)
		template<typename Type> struct PropertyMapIdx			{ static const int index = -1; }; // fallback, should produce error
		template<> struct PropertyMapIdx<StylePropertyFloat>	{ static const int index = 0; };
		template<> struct PropertyMapIdx<StylePropertyVec4>		{ static const int index = 1; };
		template<> struct PropertyMapIdx<StylePropertyString>	{ static const int index = 2; };

		// Maps from string to property class
		typedef
			std::tuple<
			std::map<std::string, StylePropertyFloat>,
			std::map<std::string, StylePropertyVec4>,
			std::map<std::string, StylePropertyString> >
			PropertyStringTuple;
		struct PropertyStringMapping
		{
			static PropertyStringTuple value;
		};

		// Getter of default value per property class
		PropertyValue<StylePropertyFloat>::type getPropertyDefault(StylePropertyFloat t);
		PropertyValue<StylePropertyVec4>::type getPropertyDefault(StylePropertyVec4 t);
		PropertyValue<StylePropertyString>::type getPropertyDefault(StylePropertyString t);
	}
}

#endif // STYLE_DEFINITIONS

