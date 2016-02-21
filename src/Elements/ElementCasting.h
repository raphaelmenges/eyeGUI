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
	static IconInteractiveElement* toIconInteractiveElement(Element* pElement)
	{
		if (pElement != NULL)
		{
			Element::Type type = pElement->getType();
			if (type == Element::Type::ICON_INTERACTIVE_ELEMENT
				|| type == Element::Type::BUTTON
				|| type == Element::Type::CIRCLE_BUTTON
				|| type == Element::Type::BOX_BUTTON
				|| type == Element::Type::DROP_BUTTON
				|| type == Element::Type::SENSOR)
			{
				return static_cast<IconInteractiveElement*>(pElement);
			}
		}
		return NULL;
	}

    // Tries to cast pointer, returns NULL if fails
    static InteractiveElement* toInteractiveElement(Element* pElement)
    {
        if (pElement != NULL)
        {
			// Is it icon interactive element?
			IconInteractiveElement* pIconInteractiveElement = toIconInteractiveElement(pElement);

			if (pIconInteractiveElement != NULL)
			{
				// Ok, cast icon interactive to interactive element pointer and return it
				return static_cast<InteractiveElement*>(pIconInteractiveElement);
			}
			else
			{
				// Test for other cases
				Element::Type type = pElement->getType();
				if (type == Element::Type::INTERACTIVE_ELEMENT
					|| type == Element::Type::KEYBOARD)
				{
					return static_cast<InteractiveElement*>(pElement);
				}
			}
        }
        return NULL;
    }

    // Tries to cast pointer, returns NULL if fails
    static NotifierElement* toNotifierElement(Element* pElement)
    {
        if (pElement != NULL)
        {
            // Is it interactive element?
            InteractiveElement* pInteractiveElement = toInteractiveElement(pElement);

            if(pInteractiveElement != NULL)
            {
                // Ok, cast interactive to notifier element pointer and return it
                return static_cast<NotifierElement*>(pInteractiveElement);
            }
            else
            {
                // Test for other cases
                Element::Type type = pElement->getType();
                if(type == Element::Type::NOTIFIER_ELEMENT)
                {
                    return static_cast<NotifierElement*>(pElement);
                }
            }
        }
        return NULL;
    }

    // Tries to cast pointer, returns NULL if fails
    static Button* toButton(Element* pElement)
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
    static Sensor* toSensor(Element* pElement)
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
    static TextBlock* toTextBlock(Element* pElement)
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

    // Tries to cast pointer, returns NULL if fails
    static Keyboard* toKeyboard(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::KEYBOARD)
            {
                return static_cast<Keyboard*>(pElement);
            }
        }
        return NULL;
    }
}

#endif // ELEMENT_CASTING_H_
