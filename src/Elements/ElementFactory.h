//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Factory class for elements. Is friend of all of them. Calls automatically
// reset after instantiation.

#ifndef ELEMENT_FACTORY_H_
#define ELEMENT_FACTORY_H_

#include <memory>

class ElementFactory
{
public:
	template <class T, class...T_values>
	std::unique_ptr<T> build(const T_values&... values)
	{
		auto upElement = std::unique_ptr<T>(new T(values...));
		upElement->reset(); // compile error if no reset method available
		return std::move(upElement);
	}
};

#endif // ELEMENT_FACTORY_H_