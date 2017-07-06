//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Maps string to style property type.

#ifndef STYLE_DEFINITIONS
#define STYLE_DEFINITIONS

#include "include/eyeGUI.h"
#include "src/Style/StyleProperty.h"

#include "externals/GLM/glm/glm.hpp"

#include <map>
#include <tuple>

namespace eyegui
{
	// ALSO TAKE A LOOK INTO CORRESPONDING CPP!

	// Compile time mapping between style property and it value type
	template<typename Type> struct StylePropertyValue;
	template<> struct StylePropertyValue<StylePropertyFloat>	{ typedef float type; };
	template<> struct StylePropertyValue<StylePropertyVec4>		{ typedef glm::vec4 type; };
	template<> struct StylePropertyValue<StylePropertyString>	{ typedef std::string type; };

	// Typedef of tuple with maps for the StyleClass
	template<typename Type> using StylePropertyMap = std::map<Type, std::shared_ptr<StyleProperty<typename StylePropertyValue<Type>::type> > >;
	typedef
		std::tuple<
			StylePropertyMap<StylePropertyFloat>,
			StylePropertyMap<StylePropertyVec4>,
			StylePropertyMap<StylePropertyString>
		> StyleMaps;

	// Indices of property types in tuple above and below
	template<typename Type> struct StylePropertyTupleIndex			{ static const int index = -1; }; // fallback, should produce error
	template<> struct StylePropertyTupleIndex<StylePropertyFloat>	{ static const int index = 0; };
	template<> struct StylePropertyTupleIndex<StylePropertyVec4>	{ static const int index = 1; };
	template<> struct StylePropertyTupleIndex<StylePropertyString>	{ static const int index = 2; };

	// TODO
	// - fill of base style class (iterating over enum options???)
	// - template specialization providing constraint per property type (before: change property types)
	// - template specialization providing parse function per property type (before: change property types)

	// Maps from string to style property type
	using StylePropertyStringMappingTuple = std::tuple<
		std::map<std::string, StylePropertyFloat>,
		std::map<std::string, StylePropertyVec4>,
		std::map<std::string, StylePropertyString> >;
	struct StylePropertyStringMappingMaps
	{
		static StylePropertyStringMappingTuple value;
	};

	// Maps to indicate default values (TODO: maybe provide fallback so not everything has to be defined? -> maybe hide behind some function
	struct StylePropertyDefaultValueMaps
	{
		static std::map<StylePropertyFloat, StylePropertyValue<StylePropertyFloat>::type> floatDefaults;
		static std::map<StylePropertyVec4, StylePropertyValue<StylePropertyVec4>::type> vec4Defaults;
		static std::map<StylePropertyString, StylePropertyValue<StylePropertyString>::type> stringDefaults;
	};

    // Move specializations to cpp as static seems not to be valid in gcc

	// Getter of maps with default values via template specialization
	template<typename Type>
    typename StylePropertyValue<Type>::type StylePropertyDefault(Type t) {}
}

#endif // STYLE_DEFINITIONS

