//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Queue for notifications sent by elements. Should be member of a layout
// and handling all notifications of its children.

#ifndef NOTIFICATION_QUEUE_H_
#define NOTIFICATION_QUEUE_H_

#include "Elements/InteractiveElements/InteractiveElement.h"

#include <memory>
#include <vector>

namespace eyegui
{
	// Forward declaration
	class Layout;

	class NotificationQueue
	{
	public:

		// Constructor
		NotificationQueue(Layout* pLayout);

		// Destructor
		virtual ~NotificationQueue();

		// Enqueue notification which is processed before next updating of layout
		void enqueue(InteractiveElement* pNotifier, InteractiveElement::Notification notification);

		// Process notifications
		void process();

	private:

		// Member
		Layout* mpLayout;
		std::unique_ptr<std::vector<std::pair<InteractiveElement*, InteractiveElement::Notification> > > mupNotificatons;
	};
}

#endif // NOTIFICATION_QUEUE_H_
