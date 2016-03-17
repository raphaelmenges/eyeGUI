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

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::BLOCK,
            meshes::Type::QUAD);
    }

    WordSuggest::~WordSuggest()
    {
        // Nothing to do
    }

    void WordSuggest::suggest(std::u16string input, Dictionary const * pDictionary)
    {
        // TODO: second parameter of similar words
        std::vector<std::u16string> suggestions = pDictionary->similarWords(input, false);
        if(!suggestions.empty())
        {
            mupSuggestion->setContent(suggestions[0]);
        }
    }

    float WordSuggest::specialUpdate(float tpf, Input* pInput)
    {
        // TODO
    }

    void WordSuggest::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("backgroundColor", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", mAlpha);
            mpBackground->getShader()->fillValue("activity", mActivity.getValue());
            mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
            mpBackground->getShader()->fillValue("dim", mDim.getValue());
            mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
            mpBackground->getShader()->fillValue("mark", mMark.getValue());
            mpBackground->draw();
        }

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
