//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "WordSuggest.h"

#include "src/Layout.h"

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
            RenderingMask::BOX,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::WORD_SUGGEST;

        // Fill members
        mFontSize = fontSize;

        // Fetch render item for background
        mpBackground = mpAssetManager->fetchRenderItem(
            shaders::Type::COLOR,
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
        mSuggestions.clear();
        for(const std::u16string& rSuggestion : suggestions)
        {
            mSuggestions.push_back(std::move(mpAssetManager->createTextSimple(mFontSize, 1, rSuggestion)));
        }
		transformSuggestions();
    }

    float WordSuggest::specialUpdate(float tpf, Input* pInput)
    {
        // TODO
		return 0;
    }

    void WordSuggest::specialDraw() const
    {
        // *** BACKGROUND ***
        if (getStyle()->backgroundColor.a > 0)
        {
            // Bind, fill and draw background
            mpBackground->bind();
            mpBackground->getShader()->fillValue("matrix", mFullDrawMatrix);
            mpBackground->getShader()->fillValue("color", getStyle()->backgroundColor);
            mpBackground->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
            mpBackground->draw();
        }

        // Draw suggestions
        for(const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
        {
            rSuggestion->draw(getStyle()->fontColor, mAlpha);
        }
    }

    void WordSuggest::specialTransformAndSize()
    {
		transformSuggestions();
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

	void WordSuggest::transformSuggestions()
	{
		int xOffset = 0;
		int xDelta = (int)((float)(mpLayout->getLayoutWidth()) * 0.005f);
		for (const std::unique_ptr<TextSimple>& rSuggestion : mSuggestions)
		{
			int width = rSuggestion->getWidth();
			rSuggestion->transform(mX + xOffset, mY);
			xOffset += width + xDelta;
		}
	}
}
