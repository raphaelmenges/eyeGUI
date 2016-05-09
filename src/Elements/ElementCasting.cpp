//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ElementCasting.h"

namespace eyegui
{
    IconElement* toIconElement(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::ICON_ELEMENT
                || type == Element::Type::BUTTON
                || type == Element::Type::CIRCLE_BUTTON
                || type == Element::Type::BOX_BUTTON
                || type == Element::Type::DROP_BUTTON
                || type == Element::Type::SENSOR)
            {
                return static_cast<IconElement*>(pElement);
            }
        }
        return NULL;
    }

    Keyboard* toKeyboard(Element* pElement)
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

    WordSuggest* toWordSuggest(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::WORD_SUGGEST)
            {
                return static_cast<WordSuggest*>(pElement);
            }
        }
        return NULL;
    }

    InteractiveElement* toInteractiveElement(Element* pElement)
    {
        if (pElement != NULL)
        {
            // Is it icon element?
            IconElement* pIconElement = toIconElement(pElement);

            if (pIconElement != NULL)
            {
                // Ok, cast icon to interactive element pointer and return it
                return static_cast<InteractiveElement*>(pIconElement);
            }
            else
            {
                // Test for other cases
                Element::Type type = pElement->getType();
                if (type == Element::Type::INTERACTIVE_ELEMENT
                    || type == Element::Type::WORD_SUGGEST
                    || type == Element::Type::KEYBOARD)
                {
                    return static_cast<InteractiveElement*>(pElement);
                }
            }
        }
        return NULL;
    }

    NotifierElement* toNotifierElement(Element* pElement)
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

    Flow* toFlow(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::FLOW)
            {
                return static_cast<Flow*>(pElement);
            }
        }
        return NULL;
    }

    Stack* toStack(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::STACK)
            {
                return static_cast<Stack*>(pElement);
            }
        }
        return NULL;
    }

    Picture* toPicture(Element* pElement)
    {
        if (pElement != NULL)
        {
            Element::Type type = pElement->getType();
            if (type == Element::Type::PICTURE)
            {
                return static_cast<Picture*>(pElement);
            }
        }
        return NULL;
    }
}
