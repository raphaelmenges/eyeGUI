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
#include <memory>

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
		// TODO

		// Reset values
		upElement->reset(); // compile error if no reset method available

		// Return element
		return std::move(upElement);
	}

private:

	// Parsing of properties
	// TODO

};

#endif // ELEMENT_FACTORY_H_