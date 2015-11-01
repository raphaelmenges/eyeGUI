//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Casting element pointers using the type information.

#ifndef ELEMENT_CASTING_H_
#define ELEMENT_CASTING_H_

#include "Elements/Elements.h"

namespace eyegui
{
	// Tries to cast pointer, returns NULL if fails
	InteractiveElement* toInteractiveElement(Element* pElement)
	{
		if (pElement != NULL)
		{
			Element::Type type = pElement->getType();
			if (type == Element::Type::INTERACTIVE_ELEMENT
				|| type == Element::Type::BUTTON
				|| type == Element::Type::CIRCLE_BUTTON
				|| type == Element::Type::BOX_BUTTON
				|| type == Element::Type::DROP_BUTTON
				|| type == Element::Type::SENSOR)
			{
				return static_cast<InteractiveElement*>(pElement);
			}
		}
		return NULL;
	}

	// Tries to cast pointer, returns NULL if fails
	Button* toButton(Element* pElement)
	{
		if (pElement != NULL)
		{
			Element::Type type = pElement->getType();
			if (type == Element::Type::BUTTON
				|| type == Element::Type::CIRCLE_BUTTON
				|| type == Element::Type::BOX_BUTTON
				|| type == Element::Type::DROP_BUTTON)
			{
				return static_cast<Button*>(pElement);
			}
		}
		return NULL;
	}

	// Tries to cast pointer, returns NULL if fails
	Sensor* toSensor(Element* pElement)
	{
		if (pElement != NULL)
		{
			Element::Type type = pElement->getType();
			if (type == Element::Type::SENSOR)
			{
				return static_cast<Sensor*>(pElement);
			}
		}
		return NULL;
	}

	// Tries to cast pointer, returns NULL if fails
	TextBlock* toTextBlock(Element* pElement)
	{
		if (pElement != NULL)
		{
			Element::Type type = pElement->getType();
			if (type == Element::Type::TEXT_BLOCK)
			{
				return static_cast<TextBlock*>(pElement);
			}
		}
		return NULL;
	}
}

#endif // ELEMENT_CASTING_H_
