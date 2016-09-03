// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKeyboard.h"

#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"

#include <string>
#include <set>

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
		bool adaptiveScaling,
		Mode mode) : InteractiveElement(
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
        mMode = mode;
        mCurrentWord = u"";
        mCollectedWords = u"";
        mLastLetter = u"";
		mLastLine = 0;
		mLastWasSuggestion = false;
		mWordBeforeSuggestion = u"";
		mCollectedWordsBeforeSuggestion = u"";

		// Initialize suggestion line
		mspSuggestionA = std::shared_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		mspSuggestionB = std::shared_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		mspSuggestionC = std::shared_ptr<FutureSuggestion>(new FutureSuggestion(mpLayout, mpAssetManager, 1.f));
		mSuggestionList.push_back(mspSuggestionA);
		mSuggestionList.push_back(mspSuggestionB);
		mSuggestionList.push_back(mspSuggestionC);

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
        for(auto& rspKey : mKeyList)
        {
			// TODO: ok, one should use upper case in standard scenario but for now we need lower
            rspKey->setCase(KeyboardCase::LOWER);
        }

        // Diplay
        mupDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);
		updateDisplay();

        // Pre display
        mupPreDisplay = mpAssetManager->createTextFlow(FontSize::MEDIUM, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP);

		// Ask initially for suggestions
		updateSuggestions();

		// Placeholder for empty suggestion
		mpEmptySuggestion = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);

		// Interaction logging
		mLastFocused.clear();
	}

	FutureKeyboard::~FutureKeyboard()
	{
		// Nothing to do
	}

	void FutureKeyboard::setSuggestionLine(std::u16string suggestionA, std::u16string suggestionB, std::u16string suggestionC)
	{
		mspSuggestionA->setSuggestion(suggestionA);
		mspSuggestionB->setSuggestion(suggestionB);
		mspSuggestionC->setSuggestion(suggestionC);
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
		// TODO: why cannot be called reset here?

		// Setup display
		mCurrentWord.clear();
		mCollectedWords.clear();
		mupPreDisplay->setContent(sentence);

		// Reset undo stuff
		mLastWasSuggestion = false;
		mWordBeforeSuggestion = u"";
		mCollectedWordsBeforeSuggestion = u"";

		// Setup suggestion line
		for (auto& rspSuggestion : mSuggestionList)
		{
			rspSuggestion->clearSuggestion();
		}

		// Setup keys
		for (auto& rspKey : mKeyList)
		{
			rspKey->clearSuggestion();
		}

		// Update display
		updateDisplay();

		// Update suggestions
		updateSuggestions();

		// Notify about interaction
		notifyInteraction("NEXT_SENTENCE", convertUTF16ToUTF8(sentence));
	}

	float FutureKeyboard::specialUpdate(float tpf, Input* pInput)
	{
        // Prepare tasks
		enum class Task { TOGGLE_CASE, LOWER_CASE, UPDATE_SUGGESTIONS };
        std::set<Task> tasks;

		// *** UPDATE OF SUGGESTIONS ***

		// Update suggestions
		for (auto& rspSuggestion : mSuggestionList)
		{
			if(rspSuggestion->update(tpf, pInput))
			{
				// Remember suggestion
				mWordBeforeSuggestion = mCurrentWord;
				mCollectedWordsBeforeSuggestion = mCollectedWords;
				mLastWasSuggestion = true;

				// Replace current word with suggestion
				mCurrentWord = rspSuggestion->getSuggestion();

				// Append space to collected and clear current word
				mCollectedWords.append(mCurrentWord + u" ");
				mCurrentWord = u"";

				// Tasks
				tasks.insert(Task::UPDATE_SUGGESTIONS);
				tasks.insert(Task::LOWER_CASE);

				// Notify about interaction
				std::string suggestionField;
				if (rspSuggestion.get() == mspSuggestionA.get())
				{
					suggestionField = "MIDDLE";
				}
				else if(rspSuggestion.get() == mspSuggestionB.get())
				{
					suggestionField = "LEFT";
				}
				else
				{
					suggestionField = "RIGHT";
				}
				notifyInteraction("HIT_SUGGESTION_" + suggestionField, convertUTF16ToUTF8(rspSuggestion->getSuggestion()));
			}
		}

		// *** UPDATE OF KEYS ***

        // Go over keys and update them
        for(auto& rspKey : mKeyList)
        {
			// Decide threshold multiplier
			float thresholdMultiplier = 1.f;
			if (rspKey->getId() == "repeat")
			{
				thresholdMultiplier = mpLayout->getConfig()->futureKeyboardRepeatKeyThresholdMultiplier;
			}
			else if (rspKey->getId() == "space")
			{
				thresholdMultiplier = mpLayout->getConfig()->futureKeyboardSpaceKeyThresholdMultiplier;
			}
			else if (rspKey->getId() == "backspace")
			{
				thresholdMultiplier = mpLayout->getConfig()->futureKeyboardBackspaceKeyThresholdMultiplier;
			}

            // Record whether hit
            FutureKey::HitType type = rspKey->update(tpf, pInput, thresholdMultiplier);

            // *** LETTERS ***
            if(type == FutureKey::HitType::LETTER
            && rspKey->getId() != "return"
            && rspKey->getId() != "backspace"
            && rspKey->getId() != "shift"
            && rspKey->getId() != "repeat"
            && rspKey->getId() != "space"
			&& rspKey->getId() != "dot")
            {
                // Seems to be standard letter, just add it to content
                mCurrentWord.append(rspKey->getLetter());

				// Remember last letter in lower case
				std::u16string lowerLastLetter = rspKey->getLetter();
				toLower(lowerLastLetter);
                mLastLetter = lowerLastLetter; // remember last letter for repeating

				// Make new suggestions
				tasks.insert(Task::UPDATE_SUGGESTIONS);
            }

            // *** SUGGESTIONS ***
            if(type == FutureKey::HitType::SUGGESTION)
            {
				// Remember suggestion
				mWordBeforeSuggestion = mCurrentWord;
				mCollectedWordsBeforeSuggestion = mCollectedWords;
				mLastWasSuggestion = true;

				// Replace current word with suggestion and display it
                mCurrentWord = rspKey->getSuggestion();                

				// Append space to collected and clear current word
				mCollectedWords.append(mCurrentWord + u" ");
				mCurrentWord = u"";

				// Update all suggestions
				tasks.insert(Task::UPDATE_SUGGESTIONS);
            } 

            // *** SPECIAL LETTERS ***

			// Shift
			if (type == FutureKey::HitType::LETTER && rspKey->getId() == "shift")
			{
				tasks.insert(Task::TOGGLE_CASE);
			}

            // Space
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "space")
            {
				// Since TextFlow is at the moment not capable to display more
				// than one space in a row, only allow one in a row
				std::u16string content = buildContent();
				char16_t compareValue = u' ';
				if ((!content.empty() && (content.back() != compareValue)) || content.empty())
				{
					// Append space to content
					mCollectedWords.append(mCurrentWord + u" ");
					mCurrentWord = u"";

					// Update all suggestions
					tasks.insert(Task::UPDATE_SUGGESTIONS);
				}
            }

            // Dot
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "dot")
            {
                mCollectedWords.append(mCurrentWord + u". ");
                mCurrentWord = u"";

				// Update all suggestions
				tasks.insert(Task::UPDATE_SUGGESTIONS);
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
					mCollectedWords = mCollectedWords.substr(0, mCollectedWords.size() - 1);

					// Try to get last word from collected words and set it as current word
					if (!mCollectedWords.empty())
					{
						// Go through words in collected
						std::u16string delimiter = u" ";
						size_t pos = 0;
						std::u16string copyCollectedWords = mCollectedWords;
						mCollectedWords = u"";
						while ((pos = copyCollectedWords.find(delimiter)) != std::u16string::npos)
						{
							mCollectedWords.append(copyCollectedWords.substr(0, pos) + u" "); // would have probably problems with mutliple spaces
							copyCollectedWords.erase(0, pos + delimiter.length());
						}
						mCurrentWord = copyCollectedWords;
					}
				}

				// Update all suggestions
				tasks.insert(Task::UPDATE_SUGGESTIONS);
            }

            // Repeat
            if(type == FutureKey::HitType::LETTER && rspKey->getId() == "repeat")
            {
                mCurrentWord.append(mLastLetter);

				// Update all suggestions
				tasks.insert(Task::UPDATE_SUGGESTIONS);
            }

            // Reset second threshold of all but current one
            if(type != FutureKey::HitType::NONE)
            {
                for(auto& rspOtherKey : mKeyList)
                {
                    if(rspOtherKey->getId() != rspKey->getId())
                    {
                        rspOtherKey->backToFirstThreshold();
                    }
                }

				// Set all letters to lower case
				if (rspKey->getId() != "shift") { tasks.insert(Task::LOWER_CASE); }
            }

			// When any letter was typed in, forget about undoing last suggestion
			if (type == FutureKey::HitType::LETTER)
			{
				// Normal letter typed in, no suggestion
				mLastWasSuggestion = false;
			}

			// Notify about interaction
			std::u16string upperId16 = convertUTF8ToUTF16(rspKey->getId());
			toUpper(upperId16);
			std::string upperId = convertUTF16ToUTF8(upperId16);
			if (type == FutureKey::HitType::LETTER)
			{
				notifyInteraction("HIT_LETTER_ON_KEY_" + upperId, "");
			}
			if (type == FutureKey::HitType::SUGGESTION)
			{
				notifyInteraction("HIT_SUGGESTION_ON_KEY_" + upperId, convertUTF16ToUTF8(rspKey->getSuggestion()));
			}

        } // end of loop over all keys

		// Update last letter on repeat key
		mspRepeatKey->setInfo(mLastLetter);

		// Some information should be only in new mode available
		if (mMode == Mode::SUGGESTION_PER_KEY)
		{
			// Update word on space key
			mspSpaceKey->setInfo(mCurrentWord);

			// Update word on backspace key
			if (mCurrentWord.empty())
			{
				mspBackspaceKey->setInfo(u"_");
			}
			else
			{
				std::u16string backWord = mCurrentWord.substr(0, mCurrentWord.size() - 1);
				backWord.append(u"_");
				mspBackspaceKey->setInfo(backWord);
			}
		}

		// Execute tasks after updating
		for (auto task : tasks)
		{
			switch (task)
			{
			case Task::LOWER_CASE:
				for (auto& rspKey : mKeyList)
				{
					rspKey->setCase(eyegui::KeyboardCase::LOWER);
				}
				break;
			case Task::TOGGLE_CASE: // executing toggle case after lower case does the trick with shift being a key but also toggling case
				for (auto& rspKey : mKeyList)
				{
					rspKey->toggleCase();
				}
				break;
			case Task::UPDATE_SUGGESTIONS:
				
				// Update suggestions
				for (auto& rspSuggestion : mSuggestionList)
				{
					rspSuggestion->clearSuggestion();
				}
				for (auto& rspKey : mKeyList)
				{
					rspKey->clearSuggestion();
				}
				updateSuggestions();

				// Determine line of last letter
				for (int i = 0; i < (int)mKeyList.size(); i++)
				{
					if (mKeyList.at(i)->getLetter() == mLastLetter)
					{
						int oldLastLine = mLastLine;
						if (i < 10)
						{
							mLastLine = 0;
						}
						else if (i < 20)
						{
							mLastLine = 1;
						}
						else
						{
							mLastLine = 2;
						}

						// TODO FILTHY! hack that transformation of suggestion line is triggered
						if (mMode == Mode::MANY_SUGGESTION_LINES && oldLastLine != mLastLine)
						{
							specialTransformAndSize();

							// Notify about change
							notifyInteraction("SUGGESTION_LINE_POSITION", std::to_string(mLastLine));
						}

						break;
					}
				}

				break;
			}
		}

		// Update display
		updateDisplay();

		// *** INTERACTION LOGGING ***
		if (pInput != NULL) // TODO gaze used?
		{
			// Candiate
			std::string focusCandidate = "";

			// Go over suggestion line
			if (pInput->gazeX >= mspSuggestionA->getX()
				&& pInput->gazeX < mspSuggestionA->getX() + mspSuggestionA->getWidth()
				&& pInput->gazeY >= mspSuggestionA->getY()
				&& pInput->gazeY < mspSuggestionA->getY() + mspSuggestionA->getHeight())
			{
				focusCandidate = "SUGGESTION_A";
			}
			if (pInput->gazeX >= mspSuggestionB->getX()
				&& pInput->gazeX < mspSuggestionB->getX() + mspSuggestionB->getWidth()
				&& pInput->gazeY >= mspSuggestionB->getY()
				&& pInput->gazeY < mspSuggestionB->getY() + mspSuggestionB->getHeight())
			{
				focusCandidate = "SUGGESTION_B";
			}
			if (pInput->gazeX >= mspSuggestionC->getX()
				&& pInput->gazeX < mspSuggestionC->getX() + mspSuggestionC->getWidth()
				&& pInput->gazeY >= mspSuggestionC->getY()
				&& pInput->gazeY < mspSuggestionC->getY() + mspSuggestionC->getHeight())
			{
				focusCandidate = "SUGGESTION_C";
			}

			// Go over keys
			for (const auto& rspKey : mKeyList)
			{
				if (pInput->gazeX >= rspKey->getX()
					&& pInput->gazeX < rspKey->getX() + rspKey->getWidth()
					&& pInput->gazeY >= rspKey->getY()
					&& pInput->gazeY < rspKey->getY() + rspKey->getHeight())
				{
					std::u16string upperId16 = convertUTF8ToUTF16(rspKey->getId());
					toUpper(upperId16);
					std::string upperId = convertUTF16ToUTF8(upperId16);
					focusCandidate = "KEY_" + upperId;
				}
			}

			// Check for focus over display
			if (pInput->gazeY < mDisplayLowerBound)
			{
				focusCandidate = "DISPLAY";
			}

			// Check, whether focus has changed
			if (focusCandidate != mLastFocused)
			{
				// Tell that something was left
				if (mLastFocused != "") { notifyInteraction("GAZE_LEFT", mLastFocused); }

				//  Tell that something was entered
				if (focusCandidate != "") { notifyInteraction("GAZE_ENTERED", focusCandidate); }

				// Save candiate
				mLastFocused = focusCandidate;
			}
		}

		return 0.f;
	}

	void FutureKeyboard::specialDraw() const
	{
		// *** SUGGESTIONS ***
		for (const auto& rspSuggestion : mSuggestionList)
		{
			rspSuggestion->draw(
				getStyle()->backgroundColor,
				getStyle()->fontColor,
				getStyle()->thresholdColor,
				getMultipliedDimmedAlpha());
		}

		// *** KEYS ***
        for(const auto& rspKey : mKeyList)
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

		// *** EMPTY SUGGESTIONS ***
		if (mMode == Mode::MANY_SUGGESTION_LINES)
		{
			mpEmptySuggestion->bind();
			mpEmptySuggestion->getShader()->fillValue("color", getStyle()->backgroundColor);
			mpEmptySuggestion->getShader()->fillValue("alpha", mAlpha);

			// Draw all six placeholders
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionADrawMatrix);
			mpEmptySuggestion->draw();
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionBDrawMatrix);
			mpEmptySuggestion->draw();
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionCDrawMatrix);
			mpEmptySuggestion->draw();
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionDDrawMatrix);
			mpEmptySuggestion->draw();
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionEDrawMatrix);
			mpEmptySuggestion->draw();
			mpEmptySuggestion->getShader()->fillValue("matrix", mEmptySuggestionFDrawMatrix);
			mpEmptySuggestion->draw();
		}
	}

	void FutureKeyboard::specialTransformAndSize()
	{
		// Display is independend from everything else
		int displayX = (int)(mX + 0.05f * mWidth);
		int displayY = mMode == Mode::MANY_SUGGESTION_LINES ? (int)(mY + 0.03f * mHeight) : (int)(mY + 0.1f * mHeight);
		int displayHeight = (int)(0.175f * mHeight);
		mDisplayLowerBound = displayY + displayHeight; // save lower bound of display to determine at interaction logging when gaze is upon it

		// Values for keys
		int keyWidth = (int)(0.09f * mWidth);
		int keyHeight = (int)(0.125f * mHeight);
		int keySpace = mMode == Mode::MANY_SUGGESTION_LINES ? (int)(0.005f * mWidth) : (int)(0.01f * mWidth);
		int keyOffsetY = mMode == Mode::MANY_SUGGESTION_LINES ? (int)(0.225f * mHeight) : (int)(0.4f * mHeight); // defines where keys star

		// Offset for suggestions and keys to start
		int xMargin = (mWidth - (10.f * keyWidth + 9.f * keySpace)) / 2.f;

		// Values for suggestion line
		int suggestionWidth = (int)((mWidth - ((2.f * xMargin) + (2.f * keySpace))) / 3.f);
		int suggestionHeight = (int)(0.1f * mHeight);
		int suggestionOffsetY = keyOffsetY - keySpace - suggestionHeight;

		// Move suggestion to the correct line
		if (mMode == Mode::MANY_SUGGESTION_LINES)
		{
			// Move it depending from last line
			suggestionOffsetY += mLastLine * (suggestionHeight + (2 * keySpace) + keyHeight);
		}
		else
		{
			// Move suggestion line a little bit up to improve selection by user
			suggestionOffsetY -= mHeight * 0.025f;
		}

		// Position empty suggestion placeholders
		if (mMode == Mode::MANY_SUGGESTION_LINES)
		{
			// Prepare some values
			int lW = mpLayout->getLayoutWidth();
			int lH = mpLayout->getLayoutHeight();
			int x1 = xMargin + mX;
			int x2 = xMargin + mX + suggestionWidth + keySpace;
			int x3 = xMargin + mX + (2 * suggestionWidth) + (2 * keySpace);
			int initialY = keyOffsetY - keySpace - suggestionHeight;
			int firstY, secondY;

			if (mLastLine == 0)
			{
				firstY  = initialY + 1 * (suggestionHeight + (2 * keySpace) + keyHeight);
				secondY = initialY + 2 * (suggestionHeight + (2 * keySpace) + keyHeight);
			}
			else if (mLastLine == 1)
			{
				firstY  = initialY + 0 * (suggestionHeight + (2 * keySpace) + keyHeight);
				secondY = initialY + 2 * (suggestionHeight + (2 * keySpace) + keyHeight);
			}
			else
			{
				firstY  = initialY + 0 * (suggestionHeight + (2 * keySpace) + keyHeight);
				secondY = initialY + 1 * (suggestionHeight + (2 * keySpace) + keyHeight);
			}

			mEmptySuggestionADrawMatrix = calculateDrawMatrix(lW, lH, x1, firstY, suggestionWidth, suggestionHeight);
			mEmptySuggestionBDrawMatrix = calculateDrawMatrix(lW, lH, x2, firstY, suggestionWidth, suggestionHeight);
			mEmptySuggestionCDrawMatrix = calculateDrawMatrix(lW, lH, x3, firstY, suggestionWidth, suggestionHeight);
			mEmptySuggestionDDrawMatrix = calculateDrawMatrix(lW, lH, x1, secondY, suggestionWidth, suggestionHeight);
			mEmptySuggestionEDrawMatrix = calculateDrawMatrix(lW, lH, x2, secondY, suggestionWidth, suggestionHeight);
			mEmptySuggestionFDrawMatrix = calculateDrawMatrix(lW, lH, x3, secondY, suggestionWidth, suggestionHeight);
		}

		// Display
		mupDisplay->transformAndSize(displayX, displayY, mWidth, displayHeight);

		// Pre display
		mupPreDisplay->transformAndSize(displayX, displayY, mWidth, displayHeight);

		// Transform and size suggestions (put a in the middle)
		mspSuggestionB->transformAndSize(xMargin + mX, suggestionOffsetY, suggestionWidth, suggestionHeight);
		mspSuggestionA->transformAndSize(xMargin + mX + suggestionWidth + keySpace, suggestionOffsetY, suggestionWidth, suggestionHeight);
		mspSuggestionC->transformAndSize(xMargin + mX + (2 * suggestionWidth) + (2 * keySpace), suggestionOffsetY, suggestionWidth, suggestionHeight);

        // First row
        mspQKey->transformAndSize(xMargin + mX,                               keyOffsetY + mY, keyWidth, keyHeight);
        mspWKey->transformAndSize(xMargin + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspEKey->transformAndSize(xMargin + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspRKey->transformAndSize(xMargin + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspTKey->transformAndSize(xMargin + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspYKey->transformAndSize(xMargin + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspUKey->transformAndSize(xMargin + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspIKey->transformAndSize(xMargin + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspOKey->transformAndSize(xMargin + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);
        mspPKey->transformAndSize(xMargin + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY, keyWidth, keyHeight);

		// Adapt key offset
		if (mMode == Mode::MANY_SUGGESTION_LINES) { keyOffsetY += suggestionHeight + keySpace; }

        // Second row
        mspAKey    ->transformAndSize(xMargin + mX,                               keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspSKey    ->transformAndSize(xMargin + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspDKey    ->transformAndSize(xMargin + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspFKey    ->transformAndSize(xMargin + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspGKey    ->transformAndSize(xMargin + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspHKey    ->transformAndSize(xMargin + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspJKey    ->transformAndSize(xMargin + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspKKey    ->transformAndSize(xMargin + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspLKey    ->transformAndSize(xMargin + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);
        mspEnterKey->transformAndSize(xMargin + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY + keyHeight + keySpace, keyWidth, keyHeight);

		// Adapt key offset
		if (mMode == Mode::MANY_SUGGESTION_LINES) { keyOffsetY += suggestionHeight + keySpace; }

        // Third row
        mspZKey        ->transformAndSize(xMargin + mX,                               keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspXKey        ->transformAndSize(xMargin + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCKey        ->transformAndSize(xMargin + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspVKey        ->transformAndSize(xMargin + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBKey        ->transformAndSize(xMargin + mX + (4 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspNKey        ->transformAndSize(xMargin + mX + (5 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspMKey        ->transformAndSize(xMargin + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspBackspaceKey->transformAndSize(xMargin + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + (2 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);

        // Forth row
        mspShiftKey      ->transformAndSize(xMargin + mX,                               keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspRepeatKey     ->transformAndSize(xMargin + mX + (1 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspCommaKey      ->transformAndSize(xMargin + mX + (2 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspSpaceKey      ->transformAndSize(xMargin + mX + (3 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), (3 * keyWidth) + (2 * keySpace), keyHeight);
        mspDotKey        ->transformAndSize(xMargin + mX + (6 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspQuestionKey   ->transformAndSize(xMargin + mX + (7 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspExclamationKey->transformAndSize(xMargin + mX + (8 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
        mspColonKey      ->transformAndSize(xMargin + mX + (9 * (keyWidth + keySpace)), keyOffsetY + mY + (3 * (keyHeight + keySpace)), keyWidth,                        keyHeight);
	}

	void FutureKeyboard::specialReset()
	{
		// Reset content
		mCurrentWord.clear();
		mCollectedWords.clear();

		// Go over suggestions and reset them
		for (const auto& rspSuggestion : mSuggestionList)
		{
			rspSuggestion->reset();
		}

		// Go over keys and reset them
		for (auto& rspKey : mKeyList)
		{
			rspKey->reset();
		}

		// Reset last letter
		mLastLetter = u"";
		mspRepeatKey->setInfo(mLastLetter);

		// Reset last line
		mLastLine = 0;

		// Undo of suggestions
		mLastWasSuggestion = false;
		mWordBeforeSuggestion = u"";
		mCollectedWordsBeforeSuggestion = u"";

		// Interaction logging
		mLastFocused.clear();
	}

	void FutureKeyboard::specialInteract()
	{

	}

	void FutureKeyboard::specialPipeNotification(NotificationType notification, Layout* pLayout)
	{
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        case NotificationType::FUTURE_KEYBOARD_NEEDS_SUGGESTIONS:
        {
			// Suggestions in suggestion line
			notifyListener(
				&eyegui_experimental::FutureKeyboardListener::lineSuggestions,
				pLayout,
				getId());

            // Go over all keys which need suggestions and ask for one
            for(auto& rspKey : mKeyList)
            {
                // Only do so for keys which want a suggestion and which are currently not at second threshold
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

    void FutureKeyboard::updateDisplay()
    {
        mupDisplay->setContent(buildContent() + u"_");        
    }

	void FutureKeyboard::updateSuggestions()
	{
		mpNotificationQueue->enqueue(getId(), NotificationType::FUTURE_KEYBOARD_NEEDS_SUGGESTIONS);
	}

	std::u16string FutureKeyboard::buildContent() const
	{
		return mCollectedWords + mCurrentWord;
	}
}
