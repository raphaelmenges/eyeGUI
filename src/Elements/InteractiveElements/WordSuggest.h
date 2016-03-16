//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef WORD_SUGGEST_H_
#define WORD_SUGGEST_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"

#include "src/Utilities/Dictionary.h"

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

        // Suggest
        void suggest(std::u16string input, Dictionary const * pDictionary);

        // TODO
        // - Background
        // - Set input word via interface
        // - Display suggestions (update / draw method necessary)
        // - Notification when word is chosen

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

        // Interaction fill by subclasses
        virtual void specialInteract();

        // Filled by subclass and called by layout after updating and before drawing
        virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

    private:

        // Members
        FontSize mFontSize;
        std::unique_ptr<TextSimple> mupSuggestion;

    };
}

#endif // WORD_SUGGEST_H_
