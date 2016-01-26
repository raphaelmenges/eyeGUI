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
    enum class NotificationType { BUTTON_HIT, BUTTON_DOWN, BUTTON_UP, SENSOR_PENETRATED, KEYBOARD_KEY_PRESSED};

    template <class T>
    class NotifierTemplate
    {
    public:

        // Constructors
        NotifierTemplate() {}

        // Destructor
        virtual ~NotifierTemplate() = 0;

        // Register listeners for pressing etc
        bool registerListener(std::weak_ptr<T> wpListener)
        {
            bool success = true;

            // Get raw pointer from weak pointer
            T* ptr = NULL;
            if (auto wp = wpListener.lock())
            {
                ptr = wp.get();
            }

            // Only continue if there is still a pointer
            if (ptr != NULL)
            {
                // Check whether listener is already listening
                for (std::weak_ptr<T>& listener : mListeners)
                {
                    // Compare raw pointer for equality
                    if (auto sp = listener.lock())
                    {
                        if (sp.get() == ptr)
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

            return success;
        }

    protected:

        // Notify listener about something (one need to know which method to call from listener)
        void notifyListener(
            void (T::*method) (Layout*, std::string),
            Layout* pLayout, std::string id)
        {
            // Inform listener
            for (std::shared_ptr<T>& spListener : getListener())
            {
                (spListener.get()->*method)(pLayout, id);
            }
        }

        // Notify listener about something (one need to know which method to call from listener)
        void notifyListener(
            void (T::*method) (Layout*, std::string, float),
            Layout* pLayout, std::string id, float value)
        {
            // Inform listener
            for (std::shared_ptr<T>& spListener : getListener())
            {
                (spListener.get()->*method)(pLayout, id, value);
            }
        }

        // Notify listener about something (one need to know which method to call from listener)
        void notifyListener(
            void (T::*method) (Layout*, std::string, std::u16string),
            Layout* pLayout, std::string id, std::u16string value)
        {
            // Inform listener
            for (std::shared_ptr<T>& spListener : getListener())
            {
                (spListener.get()->*method)(pLayout, id, value);
            }
        }

		// Notify listener about something (one need to know which method to call from listener)
		void notifyListener(
			void (T::*method) (Layout*, std::string, std::string),
			Layout* pLayout, std::string id, std::string value)
		{
			// Inform listener
			for (std::shared_ptr<T>& spListener : getListener())
			{
				(spListener.get()->*method)(pLayout, id, value);
			}
		}

    private:

        // Helper for notifications (shared pointer somehow expensive)
        std::vector<std::shared_ptr<T> > getListener()
        {
            std::vector<std::shared_ptr<T> > listeners;
            std::vector<int> deadListeners;

            // Inform listener
            for (int i = 0; i < mListeners.size(); i++)
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
                    // Dead listeners even not added to return vector
                    deadListeners.push_back(i);
                }
            }

            // Delete dead listeners from member vector
            for (int i = 0; i < deadListeners.size(); i++)
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
