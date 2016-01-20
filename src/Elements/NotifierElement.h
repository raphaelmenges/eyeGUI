//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract superclass for elements, that are able to pipe notifications. Is
// itself not subclass of notifier template, but is common superclass of those
// classes. Subclasses of the notifier template must use their specific listener
// classes as parameter.

#ifndef NOTIFIER_ELEMENT_H_
#define NOTIFIER_ELEMENT_H_

#include "Element.h"
#include "NotifierTemplate.h"

namespace eyegui
{
    class NotifierElement : public Element
    {
        public:

            // Constructors
            NotifierElement(
                std::string id,
                std::string styleName,
                Element* pParent,
                Layout const * pLayout,
                Frame* pFrame,
                AssetManager* pAssetManager,
                NotificationQueue* pNotificationQueue,
                float relativeScale,
                float border,
                bool dimming,
                bool adaptiveScaling);

            // Destructor
            virtual ~NotifierElement() = 0;

            // Called by layout after updating
            void pipeNotification(NotificationType notification, Layout* pLayout);

        protected:

            // Filled by subclass and called by layout after updating
            virtual void specialPipeNotification(NotificationType notification, Layout* pLayout) = 0;
    };
}

#endif // NOTIFIER_ELEMENT_H_
