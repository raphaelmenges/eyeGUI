//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract template for managing listeners. Specialization of interactive
// elements are deriving from this and using their specific listener structures
// as input for the template. Inside their "pipeNotification" method they call
// the "notifyListener" method with the method of their listener structure
// which should be called back for notification. Vector of weak pointers
// to the listeners is handled here and dead weak pointer are deleted
// automatically.

#ifndef NOTIFIER_TEMPLATE_H_
#define NOTIFIER_TEMPLATE_H_

#include <memory>
#include <algorithm>

namespace eyegui
{
    // Ugly enumeration, but somehow layout must remember to call correct notification
    enum class NotificationType {
    BUTTON_HIT,
    BUTTON_DOWN,
    BUTTON_UP,
    SENSOR_PENETRATED,
	KEYBOARD_KEY_SELECTED,
    KEYBOARD_KEY_PRESSED,
    WORD_SUGGEST_CHOSEN,

    // Experimental
    FUTURE_KEYBOARD_NEEDS_SUGGESTIONS };

    template <class T>
    class NotifierTemplate
    {
    public:

        // Constructors
        NotifierTemplate() {}

        // Destructor
        virtual ~NotifierTemplate() = 0;

        // Register listeners for element activities
        bool registerListener(std::weak_ptr<T> wpListener)
        {
            bool success = true;

            // Get shared pointer from weak pointer
            if (auto newSp = wpListener.lock())
            {
				// Check whether listener is already listening
				for (const std::weak_ptr<T>& rwpListener : mListeners)
				{
					// Compare raw pointers for equality
					if (auto sp = rwpListener.lock())
					{
						if (sp.get() == newSp.get())
						{
							success = false;
							break;
						}
					}
				}

				if (success)
				{
					// Not yet listening, change it
					mListeners.push_back(wpListener);
				}
            }
			else
			{
				// Could not register NULL pointer
				success = false;
			}

            return success;
        }

    protected:

        // Notify listener about something. Takes parameters of chosen method
		template <class...T_values>
        void notifyListener(
            void (T::*method) (Layout*, T_values...), // method that is called back
            Layout* pLayout, // pointer to layout of element calling
			const T_values&... values) // parameters for method
        {
            // Inform listener
            for (std::shared_ptr<T>& spListener : getListener())
            {
                (spListener.get()->*method)(pLayout, values...); // call method of element
            }
        }

    private:

        // Helper for notifications
        std::vector<std::shared_ptr<T> > getListener()
        {
            std::vector<std::shared_ptr<T> > listeners;
            std::vector<int> deadListeners;

            // Inform listener
            for (uint i = 0; i < mListeners.size(); i++)
            {
                std::weak_ptr<T>& wpListener = mListeners[i];

                // Check weak pointer getting them
                if (auto sp = wpListener.lock())
                {
                    // Add to return vector
                    listeners.push_back(sp);
                }
                else
                {
                    // Dead listeners not added to result vector
                    deadListeners.push_back(i);
                }
            }

            // Delete dead listeners from member vector
            for (uint i = 0; i < deadListeners.size(); i++)
            {
                mListeners.erase(mListeners.begin() + i);
            }

            return listeners;
        }

        // Members
        std::vector<std::weak_ptr<T> > mListeners;
    };

    template <class T>
    NotifierTemplate<T>::~NotifierTemplate()
    {
        // Empty destructor
    }
}

#endif // NOTIFIER_TEMPLATE_H_
