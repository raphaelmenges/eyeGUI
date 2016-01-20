//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Queue for notifications sent by elements. Should be member of a layout
// and handling all notifications of its notifier elements.

#ifndef NOTIFICATION_QUEUE_H_
#define NOTIFICATION_QUEUE_H_

#include "Elements/NotifierElement.h"

#include <memory>
#include <vector>

namespace eyegui
{
    // Forward declaration
    class Layout;

    // Typedefs
    typedef std::pair<std::string, NotificationType> NotificationPair;
    typedef std::vector<NotificationPair> NotificationVector;

    class NotificationQueue
    {
    public:

        // Constructor
        NotificationQueue(Layout* pLayout);

        // Destructor
        virtual ~NotificationQueue();

        // Enqueue notification which is processed before next updating of layout
        void enqueue(std::string notifierId, NotificationType notification);

        // Process notifications
        void process();

    private:

        // Member
        Layout* mpLayout;
        std::unique_ptr<NotificationVector> mupNotificatons;
    };
}

#endif // NOTIFICATION_QUEUE_H_
