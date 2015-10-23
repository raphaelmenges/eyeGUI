//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "NotificationQueue.h"

#include "Defines.h"
#include "Layout.h"
#include "OperationNotifier.h"

namespace eyegui
{
	NotificationQueue::NotificationQueue(Layout* pLayout)
	{
		mpLayout = pLayout;
		mupNotificatons = std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > >(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);
	}

	NotificationQueue::~NotificationQueue()
	{
		// Nothing to do
	}

	void NotificationQueue::enqueue(InteractiveElement* pNotifier, InteractiveElement::Notification notification)
	{
		mupNotificatons->push_back(std::pair<InteractiveElement*, InteractiveElement::Notification>(pNotifier, notification));
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
				mupNotificatons.reset(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);
				throwWarning(OperationNotifier::Operation::RUNTIME, "Notifications are too nested and were cleared");

				// Break the while loop
				break;
			}

			else
			{
				// Transfer notifications to local pointer and replace with empty one
				std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > > upNotifications = std::move(mupNotificatons);
				mupNotificatons = std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > >(new std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> >);

				// Work on notifications
				for (int i = 0; i < upNotifications->size(); i++)
				{
					// Listeners of notifications could fill the notifications in this layout again... (just a note)
					std::pair<InteractiveElement*, InteractiveElement::Notification> notification = upNotifications->at(i);

					// Piping takes care that replaced elements do not send notifications
					notification.first->pipeNotification(notification.second, mpLayout);
				}

				// count loops
				loopCount++;
			}
		}
	}
}