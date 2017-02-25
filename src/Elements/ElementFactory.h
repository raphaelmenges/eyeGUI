//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Factory class for elements. Is friend of all of them. Calls automatically
// reset after instantiation.

#ifndef ELEMENT_FACTORY_H_
#define ELEMENT_FACTORY_H_

#include "src/Defines.h"
#include "src/Style/StylePropertyNameMapper.h"
#include <memory>

namespace eyegui
{
	class ElementFactory
	{
	public:

		// Construction
		template <class T, class...T_values>
		std::unique_ptr<T> build(std::string propertiesString, const T_values&... values)
		{
			// Construct element
			auto upElement = std::unique_ptr<T>(new T(values...));

			// Parse properties and assign them (must be done after construction and before anything else!)
			if (!propertiesString.empty())
			{
				// Go through the lines
				std::string delimiter = " ";
				size_t pos = 0;
				std::string property;

				// Append last space because while loop does not read last line
				propertiesString += delimiter;

				// Go over single properties, separated by space
				while ((pos = propertiesString.find(delimiter)) != std::string::npos)
				{
					// Read property and erase it from all properties
					property = propertiesString.substr(0, pos);
					propertiesString.erase(0, pos + delimiter.length());

					// Try to set property when string it not empty
					if (!property.empty())
					{
						// Get left and right side
						std::string propertyDelimiter = "=";
						size_t propertyPos = 0;

						if ((propertyPos = property.find(propertyDelimiter)) == std::string::npos)
						{
							// TODO: warning
							continue;
						}

						std::string left = property.substr(0, propertyPos);
						property.erase(0, propertyPos + propertyDelimiter.length());
						std::string right = property;

						// Fill it into style property of elements class
						auto floatIterator = StylePropertyNameMapper::FLOAT_TYPE_MAP.find(left);
						if (floatIterator != StylePropertyNameMapper::FLOAT_TYPE_MAP.end()) { upElement->setStyleValue(floatIterator->second, right); }
						auto vec4Iterator = StylePropertyNameMapper::VEC4_TYPE_MAP.find(left);
						if (vec4Iterator != StylePropertyNameMapper::VEC4_TYPE_MAP.end()) { upElement->setStyleValue(vec4Iterator->second, right); }
					}
				}
			}

			// Reset values
			upElement->reset(); // compile error if no reset method available

			// Return element
			return std::move(upElement);
		}
	};
}

#endif // ELEMENT_FACTORY_H_