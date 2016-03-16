//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef WORD_SUGGEST_H_
#define WORD_SUGGEST_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"

namespace eyegui
{
    class WordSuggest : public InteractiveElement
    {
    public:

        // Constructor
        WordSuggest(
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
            bool adaptiveScaling,
            FontSize fontSize);

        // Destructor
        virtual ~WordSuggest();

        // TODO
        // - Background
        // - Set input word via interface
        // - Display suggestions (update / draw method necessary)
        // - Notification when word is chosen

    private:

        // Members
        FontSize mFontSize;

    };
}

#endif // WORD_SUGGEST_H_
