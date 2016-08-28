// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKeyboard.h"

#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"

#include <string>

namespace eyegui
{
	FutureKeyboard::FutureKeyboard(
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
		bool adaptiveScaling) : InteractiveElement(
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
		mType = Type::FUTURE_KEYBOARD;

        // Initialize members
        mMode = Mode::SUGGESTION_PER_KEY;
        mCurrentWord = u"";
        mCollectedWords = u"";
        mLastLetter = u"";

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Initialize keys
        std::function<std::shared_ptr<FutureKey>(std::string, std::u16string, bool, bool)> createFutureKey =
            [&](std::string id, std::u16string letter, bool showSuggestion, bool ignoreCase)
            {
                auto spNewKey = std::shared_ptr<FutureKey>(
                    new FutureKey(
                        id,
                        mpLayout,
                        mpAssetManager,
                        letter,
                        0.8f,
                        showSuggestion,
                        KeyboardCase::UPPER,
                        ignoreCase)); // create new key
                mKeyList.push_back(spNewKey); // save smart pointer to list
                return spNewKey;
            };

        // First row
        mspQKey = createFutureKey("q", u"Q", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspWKey = createFutureKey("w", u"W", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspEKey = createFutureKey("e", u"E", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspRKey = createFutureKey("r", u"R", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspTKey = createFutureKey("t", u"T", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspYKey = createFutureKey("y", u"Y", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspUKey = createFutureKey("u", u"U", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspIKey = createFutureKey("i", u"I", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspOKey = createFutureKey("o", u"O", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspPKey = createFutureKey("p", u"P", mMode == Mode::SUGGESTION_PER_KEY, false);

        // Second row
        mspAKey = createFutureKey("a", u"A", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspSKey = createFutureKey("s", u"S", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspDKey = createFutureKey("d", u"D", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspFKey = createFutureKey("f", u"F", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspGKey = createFutureKey("g", u"G", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspHKey = createFutureKey("h", u"H", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspJKey = createFutureKey("j", u"J", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspKKey = createFutureKey("k", u"K", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspLKey = createFutureKey("l", u"L", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspEnterKey = createFutureKey("return", u"\u21AA", false, true);

        // Third row
        mspZKey = createFutureKey("z", u"Z", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspXKey = createFutureKey("x", u"X", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspCKey = createFutureKey("c", u"C", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspVKey = createFutureKey("v", u"V", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspBKey = createFutureKey("b", u"B", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspNKey = createFutureKey("n", u"N", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspMKey = createFutureKey("m", u"M", mMode == Mode::SUGGESTION_PER_KEY, false);
        mspBackspaceKey = createFutureKey("backspace", u"\u21A4", false, true);

        // Forth row
        mspShiftKey = createFutureKey("shift", u"\u2191", false, true);
        mspRepeatKey = createFutureKey("repeat", u"\u21BA", false, true);
        mspCommaKey = createFutureKey("comma", u",", false, true);
        mspSpaceKey = createFutureKey("space", u"\u2423", false, true);
        mspDotKey = createFutureKey("dot", u".", false, true);
        mspQuestionKey = createFutureKey("question", u"?", false, true);
        mspExclamationKey = createFutureKey("exclamation", u"!", false, true);
        mspColonKey = createFutureKey("colon", u":", false, true);

        // For first letter, use upper case
        mFirstLetterOfSentence = true;
        for(auto& rspKey : mKeyList)
        {
            rspKey->setCase(KeyboardCase::UPPER);
        }

        // Diplay
        mupDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
        updateDisplayAndSuggestions();

        // Pre display
        mupPreDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
	}

	FutureKeyboard::~FutureKeyboard()
	{

	}

    void FutureKeyboard::setKeySuggestion(std::string keyId, std::u16string suggestion)
    {
        // Search for key
        for(auto& rspKey : mKeyList)
        {
            if(rspKey->getId() == keyId)
            {
                rspKey->setSuggestion(suggestion);
                break;
            }
        }
    }

	std::u16string FutureKeyboard::getContent() const
	{
		return buildContent();
	}

	void FutureKeyboard::nextSentence(std::u16string sentence)
	{
		// Setup display
		mCurrentWord.clear();
		mCollectedWords.clear();
		mupPreDisplay->setContent(sentence);

		// Setup keys
		for (auto& rspKey : mKeyList)
		{
			rspKey->clearSuggestion();
		}
	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        // Prepare tasks
		enum class KeyTask { TOGGLE_CASE, CLEAR_SUGGESTION };
        std::vector<KeyTask> tasks;
        bool firstLetterOfSentence = false;
        bool keyHit = false;

        // Go over keys and update them
        for(auto& rspKey : mKeyList)
        {
            // Record whether hit
            FutureKey::HitType type = rspKey->update(tpf, pInput);

            // Toggle case
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "shift")
            {
                tasks.push_back(KeyTask::TOGGLE_CASE);
            }

            // *** LETTERS ***
            if(type == FutureKey::HitType::LETTER
            && rspKey->getId() != "return"
            && rspKey->getId() != "backspace"
            && rspKey->getId() != "shift"
            && rspKey->getId() != "repeat"
            && rspKey->getId() != "space")
            {
                // Seems to be standard letter, just add it to content
                mCurrentWord.append(rspKey->getLetter());
                keyHit = true;
                mLastLetter = rspKey->getLetter();
                updateDisplayAndSuggestions();
            }

            // *** SUGGESTIONS ***
            if(type == FutureKey::HitType::SUGGESTION)
            {
				// Replace current word with suggestion and display it
                mCurrentWord = rspKey->getSuggestion();
                updateDisplayAndSuggestions();
                keyHit = true;
            }

            // *** SPECIAL LETTERS ***

            // Space
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "space")
            {
				// Clear all suggestions
				tasks.push_back(KeyTask::CLEAR_SUGGESTION);

				// Append space to content
                mCurrentWord.append(u" ");
                mCollectedWords.append(mCurrentWord);
                mCurrentWord = u"";
                updateDisplayAndSuggestions();
            }

            // Dot
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "dot")
            {
                mCurrentWord.append(u" ");
                mCollectedWords.append(mCurrentWord);
                mCurrentWord = u"";
                firstLetterOfSentence = true;
                updateDisplayAndSuggestions();
            }

            // Backspace
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "backspace")
            {
                // Try to delete something from current word
                if(!mCurrentWord.empty())
                {
					mCurrentWord = mCurrentWord.substr(0, mCurrentWord.size() - 1);
                }
                else if(!mCollectedWords.empty())
                {
					mCurrentWord = mCurrentWord.substr(0, mCurrentWord.size() - 1);
                }

                // Handle upper case at first letter of sentence
                if(mCollectedWords.empty())
                {
                    firstLetterOfSentence = true;
                }

                updateDisplayAndSuggestions();
            }

            // Repeat
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "repeat")
            {
                std::u16string lowerLastLetter = mLastLetter;
                toLower(lowerLastLetter);
                mCurrentWord.append(lowerLastLetter);
                updateDisplayAndSuggestions();
            }

            // *** RESET SECOND THRESHOLD OF ALL KEYS BUT CURRENT ONE ***
            if(type != FutureKey::HitType::NONE)
            {
                for(auto& rspOtherKey : mKeyList)
                {
                    if(rspOtherKey->getId() != rspKey->getId())
                    {
                        rspOtherKey->backToFirstThreshold();
                    }
                }
            }
        }

		/* NOT NECESSARY AT THE MOMENT
        // Handle "UPPER" letters for every sentence beginnging
        if(firstLetterOfSentence)
        {
            // Prepare for next first letter
            mFirstLetterOfSentence = true;
            for(auto& rspKey : mKeyList) // maybe move out of loop...
            {
                rspKey->setCase(KeyboardCase::UPPER);
            }
        }
        else if(keyHit)
        {
            // Check whether that letter was first of a sentence
            if(mFirstLetterOfSentence)
            {
                mFirstLetterOfSentence = false;
                for(auto& rspKey : mKeyList)
                {
                    rspKey->setCase(KeyboardCase::LOWER);
                }
            }
        }
		*/

        // Execute task after updating all keys
        for(auto task : tasks)
        {
            switch(task)
            {
                case KeyTask::TOGGLE_CASE:
                    for(auto& rspKey : mKeyList)
                    {
                        rspKey->toggleCase();
                    }
                    break;
				case KeyTask::CLEAR_SUGGESTION:
					for (auto& rspKey : mKeyList)
					{
						rspKey->clearSuggestion();
					}
					break;
            }
        }

		return 0.f;
	}

	void FutureKeyboard::specialDraw() const
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

		// *** KEYS ***
        for(auto& rspKey : mKeyList)
        {
            rspKey->draw(
                getStyle()->color,
                getStyle()->fontColor,
                getStyle()->backgroundColor,
                getStyle()->thresholdColor,
                getMultipliedDimmedAlpha());
        }

        // *** DISPLAY ***
        mupPreDisplay->draw(glm::vec4(0.4f,0.4f,0.4f,1), getMultipliedDimmedAlpha());;
        mupDisplay->draw(glm::vec4(1,1,1,1), getMultipliedDimmedAlpha());
	}

	void FutureKeyboard::specialTransformAndSize()
	{
        int xOffset = 0.01f * mWidth;
        int yOffset = 0.4f * mHeight;
        int keyWidth = 0.09f * mWidth;
        int keyHeight = 0.125f * mHeight;
        int keySpace = 0.01f * mWidth;

        // First row
        mspQKey->transformAndSize(xOffset + mX,                               yOffset + mY, keyWidth, keyHeight);
        mspWKey->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspEKey->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspRKey->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspTKey->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspYKey->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspUKey->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspIKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspOKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);
        mspPKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY, keyWidth, keyHeight);

        // Second row
        mspAKey    ->transformAndSize(xOffset + mX,                               yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspSKey    ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspDKey    ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspFKey    ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspGKey    ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspHKey    ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspJKey    ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspKKey    ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspLKey    ->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspEnterKey->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY + keyHeight + keySpace, keyWidth, keyHeight);

        // Third row
        mspZKey        ->transformAndSize(xOffset + mX,                               yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspXKey        ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCKey        ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspVKey        ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBKey        ->transformAndSize(xOffset + mX + (4 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspNKey        ->transformAndSize(xOffset + mX + (5 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspMKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBackspaceKey->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + (2 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);

        // Forth row
        mspShiftKey      ->transformAndSize(xOffset + mX,                               yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspRepeatKey     ->transformAndSize(xOffset + mX + (1 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCommaKey      ->transformAndSize(xOffset + mX + (2 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspSpaceKey      ->transformAndSize(xOffset + mX + (3 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);
        mspDotKey        ->transformAndSize(xOffset + mX + (6 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspQuestionKey   ->transformAndSize(xOffset + mX + (7 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspExclamationKey->transformAndSize(xOffset + mX + (8 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspColonKey      ->transformAndSize(xOffset + mX + (9 * (keyWidth + keySpace)), yOffset + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);

        // Display
        mupDisplay->transformAndSize(mX + 0.05f * mWidth, mY + 0.05f * mHeight, mWidth, 0.2f * mHeight);

        // Pre display
        mupPreDisplay->transformAndSize(mX + 0.05f * mWidth, mY + 0.05f * mHeight, mWidth, 0.2f * mHeight);
	}

	void FutureKeyboard::specialReset()
	{
		// Go over keys and reset them
		for (auto& rspKey : mKeyList)
		{
			rspKey->reset();
		}
	}

	void FutureKeyboard::specialInteract()
	{

	}

	void FutureKeyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
	{
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::FUTURE_KEY_NEEDS_SUGGESTION:
        {
            // Go over all keys which need suggestions and ask for one
            for(auto& rspKey : mKeyList)
            {
                // Only do so for keys which want a suggestion
				if(rspKey->suggestionShown() && rspKey->atFirstThreshold())
                {
                    notifyListener(
                        &eyegui_experimental::FutureKeyboardListener::keySuggestion,
                        pLayout,
                        getId(),
                        rspKey->getId(),
                        rspKey->getLetter());
                }
            }

            break;
        }
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Future keyboard got notification which is not thought for it.");
            break;
        }
	}

    void FutureKeyboard::updateDisplayAndSuggestions()
    {
        // *** DISPLAY ***
        mupDisplay->setContent(buildContent() + u"_");

        // *** SUGGESTIONS ***
        mpNotificationQueue->enqueue(getId(), NotificationType::FUTURE_KEY_NEEDS_SUGGESTION);
    }

	std::u16string FutureKeyboard::buildContent() const
	{
		if (mCollectedWords.empty())
		{
			return mCurrentWord;
		}
		else
		{
			return mCollectedWords + u" " + mCurrentWord;
		}
	}
}
