//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "NotificationQueue.h"

#include "Defines.h"
#include "Layout.h"
#include "OperationNotifier.h"
#include "Elements/ElementCasting.h"

namespace eyegui
{
    NotificationQueue::NotificationQueue(Layout* pLayout)
    {
        mpLayout = pLayout;
        mupNotificatons = std::unique_ptr<NotificationVector>(new NotificationVector);
    }

    NotificationQueue::~NotificationQueue()
    {
        // Nothing to do
    }

    void NotificationQueue::enqueue(std::string notifierId, InteractiveElement::Notification notification)
    {
        mupNotificatons->push_back(NotificationPair(notifierId, notification));
    }

    void NotificationQueue::process()
    {
        // Repeat working on the notifications
        int loopCount = 0;
        while (mupNotificatons->size() > 0)
        {
            if (loopCount == NOTIFICATION_MAX_LOOP_COUNT)
            {
                // Delete still existing notifications
                mupNotificatons.reset(new NotificationVector);
                throwWarning(OperationNotifier::Operation::RUNTIME, "Notifications are too nested and were cleared");

                // Break the while loop
                break;
            }

            else
            {
                // Move notifications to local unique pointer and replace with empty one, which can be filled again
                std::unique_ptr<NotificationVector> upNotifications = std::move(mupNotificatons);
                mupNotificatons = std::unique_ptr<NotificationVector>(new NotificationVector);

                // Work on notifications
                for (int i = 0; i < upNotifications->size(); i++)
                {
                    // Listeners of notifications could fill the notifications in this layout again... (just a note)
                    NotificationPair notification = upNotifications->at(i);

                    // Get interactive element by id from layout
                    InteractiveElement* pInteractiveElement = toInteractiveElement(mpLayout->fetchElement(notification.first));

                    // Piping takes care that replaced elements do not send notifications
                    pInteractiveElement->pipeNotification(notification.second, mpLayout);
                }

                // count loops
                loopCount++;
            }
        }
    }
}
