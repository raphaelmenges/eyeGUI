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
// If adding a new PropertyClass here, remember to add setter in eyeGUI.h

#ifndef STYLE_DEFINITIONS
#define STYLE_DEFINITIONS

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"
#include "src/Utilities/Helper.h"

#include "externals/GLM/glm/glm.hpp"

#include <map>
#include <tuple>

namespace eyegui
{
	namespace style
	{
		// ###################################
		// Information for each property class
		// ###################################
		template<typename Type> struct PropertyInfo;

		// Float
		template<> struct PropertyInfo<StylePropertyFloat>
		{
			typedef float type; // raw type
			static const int idx = 0; // index of map in PropertyMaps tuple
			static type constraint(type value) { return value; } // constraint function
			static type parse(std::string value) { return stringToFloat(value); } // parse function
		};

		// Vec4
		template<> struct PropertyInfo<StylePropertyVec4>
		{
			typedef glm::vec4 type; // raw type
			static const int idx = 1; // index of map in PropertyMaps tuple
			static type constraint(type value) { return value; } // constraint function
			static type parse(std::string value) { return stringHexRGBAToVec4RGBA(value); } // parse function
		};

		// String
		template<> struct PropertyInfo<StylePropertyString>
		{
			typedef std::string type; // raw type
			static const int idx = 2; // index of map in PropertyMaps tuple
			static type constraint(type value) { return value; } // constraint function
			static type parse(std::string value) { return value; } // parse function
		};
		
		// ###################################
		//    Tuple of maps for StyleClass
		// ###################################

		// Typedef of tuple containing maps of property classes for style class
		template<typename Type> using PropertyMap = std::map<Type, std::shared_ptr<StyleProperty<typename PropertyInfo<Type>::type> > >;
		typedef
			std::tuple<
			PropertyMap<StylePropertyFloat>,
			PropertyMap<StylePropertyVec4>,
			PropertyMap<StylePropertyString>
			> PropertyMaps;

		// ###################################
		//   Mapping from string to property
		// ###################################

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

		// ###################################
		//    Default values of properties
		// ###################################

		// Getter of default value for each property
		typename PropertyInfo<StylePropertyFloat>::type		getPropertyDefault(StylePropertyFloat t);
		typename PropertyInfo<StylePropertyVec4>::type		getPropertyDefault(StylePropertyVec4 t);
		typename PropertyInfo<StylePropertyString>::type	getPropertyDefault(StylePropertyString t);
	}
}

#endif // STYLE_DEFINITIONS

