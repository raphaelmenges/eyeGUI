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
    IconElement* toIconElement(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Keyboard* toKeyboard(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    WordSuggest* toWordSuggest(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    InteractiveElement* toInteractiveElement(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    NotifierElement* toNotifierElement(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Button* toButton(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Sensor* toSensor(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    TextBlock* toTextBlock(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Flow* toFlow(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Stack* toStack(Element* pElement);

    // Tries to cast pointer, returns NULL if fails
    Picture* toPicture(Element* pElement);
}

#endif // ELEMENT_CASTING_H_
