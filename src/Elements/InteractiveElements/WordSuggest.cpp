//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "WordSuggest.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
    WordSuggest::WordSuggest(
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
        FontSize fontSize) : InteractiveElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::WORD_SUGGEST;

        // Fill members
        mFontSize = fontSize;

        // Get a text simple from asset manager
        mupSuggestion = std::move(mpAssetManager->createTextSimple(mFontSize, 1.0, u"Suggestion"));
    }

    WordSuggest::~WordSuggest()
    {
        // Nothing to do
    }

    float WordSuggest::specialUpdate(float tpf, Input* pInput)
    {
        // TODO
    }

    void WordSuggest::specialDraw() const
    {
        // Draw suggestions
        mupSuggestion->draw(glm::vec4(1,1,1,1));
    }

    void WordSuggest::specialTransformAndSize()
    {
        // Transform suggestions
        mupSuggestion->transform(mX, mY);
    }

    void WordSuggest::specialReset()
    {
        // TODO
    }

    void WordSuggest::specialInteract()
    {
        // TODO
    }

    void WordSuggest::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // TODO
    }

}
