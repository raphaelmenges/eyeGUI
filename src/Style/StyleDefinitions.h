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
// If adding a new PropertyClass here, remember to add setters in eyeGUI.h

#ifndef STYLE_DEFINITIONS
#define STYLE_DEFINITIONS

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"
#include "src/Utilities/Helper.h"
#include "src/Defines.h"

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

		// Duration
		template<> struct PropertyInfo<property::Duration>
		{
			typedef float type; // raw type
			static const int idx = 0; // index of map in PropertyMaps tuple
			static type constraint(type value) { return std::move(glm::max(MINIMAL_DURATION_VALUE, value)); } // constraint function
			static type parse(std::string value) { return stringToFloat(value); } // parse function
		};

		// Percentage
		template<> struct PropertyInfo<property::Percentage>
		{
			typedef float type; // raw type
			static const int idx = 1; // index of map in PropertyMaps tuple
			static type constraint(type value) { return std::move(glm::clamp(value, 0.f, 1.f)); } // constraint function
			static type parse(std::string value) { return stringToFloat(value); } // parse function
		};

		// Amount
		template<> struct PropertyInfo<property::Amount>
		{
			typedef float type; // raw type
			static const int idx = 2; // index of map in PropertyMaps tuple
			static type constraint(type value) { return std::move(glm::max(0.f, value)); } // constraint function
			static type parse(std::string value) { return stringToFloat(value); } // parse function
		};

		// Color
		template<> struct PropertyInfo<property::Color>
		{
			typedef glm::vec4 type; // raw type
			static const int idx = 3; // index of map in PropertyMaps tuple
			static type constraint(type value) { return value; } // constraint function
			static type parse(std::string value) { return stringHexRGBAToVec4RGBA(value); } // parse function
		};

		// AssetPath
		template<> struct PropertyInfo<property::AssetPath>
		{
			typedef std::string type; // raw type
			static const int idx = 4; // index of map in PropertyMaps tuple
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
			PropertyMap<property::Duration>,
			PropertyMap<property::Percentage>,
			PropertyMap<property::Amount>,
			PropertyMap<property::Color>,
			PropertyMap<property::AssetPath>
			> PropertyMaps;

		// ###################################
		//   Mapping from string to property
		// ###################################

		// Maps from string to property class
		typedef
			std::tuple<
			std::map<std::string, property::Duration>,
			std::map<std::string, property::Percentage>,
			std::map<std::string, property::Amount>,
			std::map<std::string, property::Color>,
			std::map<std::string, property::AssetPath> >
			PropertyStringTuple;
		struct PropertyStringMapping
		{
			static PropertyStringTuple value;
		};

		// ###################################
		//    Default values of properties
		// ###################################

		// Getter of default value for each property
		typename PropertyInfo<property::Duration>::type		getPropertyDefault(property::Duration t);
		typename PropertyInfo<property::Percentage>::type	getPropertyDefault(property::Percentage t);
		typename PropertyInfo<property::Amount>::type		getPropertyDefault(property::Amount t);
		typename PropertyInfo<property::Color>::type		getPropertyDefault(property::Color t);
		typename PropertyInfo<property::AssetPath>::type	getPropertyDefault(property::AssetPath t);
	}
}

#endif // STYLE_DEFINITIONS

