//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Element which takes one input word via interface and displays suggestions.
// One can chose a suggestion by threshold and listen to it with a listener
// structure. Works good in combination with keyboard element.

#ifndef WORD_SUGGEST_H_
#define WORD_SUGGEST_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"

#include "src/Utilities/Dictionary.h"

namespace eyegui
{
    class WordSuggest : public InteractiveElement, public NotifierTemplate<WordSuggestListener>
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

        // Suggest. Best suggestion may be empty.
        void suggest(std::u16string input, Dictionary const * pDictionary, std::u16string& rBestSuggestion);

        // Clear
        void clear();

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

        // Structs
        struct ChosenSuggestion
        {
            ChosenSuggestion(float alpha, float originalY, std::unique_ptr<TextSimple> upText)
            {
                this->alpha = alpha;
                this->originalY = originalY;
                this->upText = std::move(upText);
            }

            float alpha;
            float originalY; // saved as float for animation!
            std::unique_ptr<TextSimple> upText;
        };

        // Transform the suggestions, fills members needed in position below
        void transformSuggestions();

        // Position the suggestions
        void positionSuggestions();

        // Chose suggestion
        void chooseSuggestion(int index);

        // Members
        RenderItem const * mpBackground;
        FontSize mFontSize;
        std::vector<std::unique_ptr<TextSimple> > mSuggestions;
        LerpValue mOffset; // offset given in percent of all suggestions
        int mDelta;
        int mCompleteWidth;
        std::vector<LerpValue> mThresholds;
        RenderItem const * mpThresholdItem;
        std::u16string mLastChosenSuggestion;
        std::vector<ChosenSuggestion> mChosenSuggestions;
    };
}

#endif // WORD_SUGGEST_H_
