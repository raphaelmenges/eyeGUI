//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextEdit.h"

#include "src/Layout.h"
#include "src/Rendering/ScissorStack.h"
#include "src/Utilities/Helper.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
	TextEdit::TextEdit(
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
		mType = Type::TEXT_EDIT;

		// Fill members
		mFontSize = fontSize;
		mTextFlowYOffset.setValue(0);
		mActiveWordFading = 0;
		mCursorPulse = 1.f;
		mCursorSubWordIndex = 0;

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Fetch render item for cursor
		mpCursor = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Fetch render item for background of active word
		mpActiveWordBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Create text flow
		mupTextFlow = std::move(mpAssetManager->createTextFlow(
			fontSize,
			TextFlowAlignment::LEFT,
			TextFlowVerticalAlignment::TOP,
			1.0f,
			u"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.",
			true));
	}

    TextEdit::~TextEdit()
    {
        // Nothing to do
    }

	void TextEdit::moveCursorOverLetters(int letterCount)
	{
		// When there is no active word, there is no text
		if (mupActiveWord != NULL)
		{
			// Decide direction
			bool rightward = letterCount > 0;
			for (int i = 0; i < glm::abs(letterCount); i++)
			{
				// Get index of current flow word
				int flowWordIndex = mupActiveWord->index;

				// Get letter count of current subword
				int letterCount = (int)mupActiveWord->subWords.at(mCursorSubWordIndex).lettersXOffsets.size();

				// Try to in/decrement letter index
				rightward ? mCursorLetterIndex++ : mCursorLetterIndex--;

				// Check, whether letter index is still in range
				if (mCursorLetterIndex >= letterCount || mCursorLetterIndex < -1)
				{
					// Get sub word count of active word
					int subWordCount = (int)mupActiveWord->subWords.size();

					// Try to in/decrement sub word index
					rightward ? mCursorSubWordIndex++ : mCursorSubWordIndex--;

					// Reset letter before first index
					mCursorLetterIndex = -1;

					// Check, whether sub word index is still in range
					if (mCursorSubWordIndex >= subWordCount || mCursorSubWordIndex < 0)
					{
						// Reset index to first sub word
						mCursorSubWordIndex = 0;

						// Try to get next word in flow
						TextFlow::FlowWord newActiveWord;
						if (mupTextFlow->getFlowWord(
							rightward ? flowWordIndex + 1 : flowWordIndex - 1,
							newActiveWord))
						{
							setActiveWord(newActiveWord, false);

							// Next word found, so set cursor at last subword behind last letter
							if (!rightward)
							{
								mCursorSubWordIndex = (int)mupActiveWord->subWords.size() - 1;
								mCursorLetterIndex = (int)mupActiveWord->subWords.at(mCursorSubWordIndex).lettersXOffsets.size() - 1;
							}
						}
						else
						{
							// Set to last word and last position when rightwarded, otherwise it is leftwarded and already set to first subword and before first letter
							if (rightward)
							{
								mCursorSubWordIndex = (int)mupActiveWord->subWords.size() - 1;
								mCursorLetterIndex = (int)mupActiveWord->subWords.at(mCursorSubWordIndex).lettersXOffsets.size() - 1;
							}
						}
					}
				}
			}
		}
	}

	void TextEdit::moveCursorOverWords(int wordCount)
	{
		// When there is no active word, there is no text
		if (mupActiveWord != NULL)
		{
			// Decide direction
			bool rightward = wordCount > 0;
			for (int i = 0; i < glm::abs(wordCount); i++)
			{
				// Count how many letters have to be skipped to get to next word
				int letterCount = 0;
				if (rightward)
				{
					// Collect left letters of all subwords
					for (int j = mCursorSubWordIndex; j < mupActiveWord->subWords.size(); j++)
					{
						letterCount += (int)mupActiveWord->subWords.at(j).lettersXOffsets.size();
					}
                    letterCount -= glm::max(0, mCursorLetterIndex);
				}
				else
				{
					// Collect letters of lefhand subwords
					for (int j = 0; j < mCursorSubWordIndex; j++)
					{
                        letterCount = -(int)mupActiveWord->subWords.at(j).lettersXOffsets.size();
					}
                    letterCount -= glm::max(0, mCursorLetterIndex) + 1;
				}

				// Delegate it to move by letters method
				moveCursorOverLetters(letterCount);
			}
		}
	}

	void TextEdit::addContentAtCursor(std::u16string content)
	{
		// TODO: only works when there is a active word, must also work for empty text
		if (mupActiveWord != NULL)
		{
			// Find out, where the cursor is in content
			int contentIndex = mupActiveWord->contentIndex;
			for (int i = 0; i < mCursorSubWordIndex; i++)
			{
				contentIndex += (int)mupActiveWord->subWords.at(i).lettersXOffsets.size();
			}
			contentIndex += mCursorLetterIndex; // TODO: how to handle -1 ?

			// Tell text flow to add content there
			mupTextFlow->addContent(contentIndex, content);

			// TODO: set cursor to end of insertion
		}
	}

    float TextEdit::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

		// *** ANIMATIONS ***

		// Update previous active words
		for (auto& rPair : mPreviousActiveWords)
		{
			rPair.second -= tpf;
		}
		mPreviousActiveWords.erase(
			std::remove_if(
				mPreviousActiveWords.begin(),
				mPreviousActiveWords.end(),
                [](const SubFlowWordAlphaPair& i) { return i.second <= 0; }),
			mPreviousActiveWords.end());

		// Update active word's fading
		if (mupActiveWord != NULL)
		{
			mActiveWordFading = glm::min(mActiveWordFading + tpf, mpLayout->getConfig()->animationDuration);
		}

		// Update pulsing of cursor
		float fullCircle = 2 * glm::pi<float>();
		mCursorPulse += (tpf * fullCircle) / TEXT_EDIT_CURSOR_PULSE_DURATION;
		while (mCursorPulse >= fullCircle) { mCursorPulse -= fullCircle; }

		// *** UPDATE OF TEXT FLOW ***

		// Scroll text flow
		if (penetratedByInput(pInput) && (mupTextFlow->getHeight() > 0))
		{
			// *** SCROLLING ***

			// Coordinates in flow coordinates
			int flowX = pInput->gazeX - mX;
			int flowY = pInput->gazeY - mY;

			// (Old) y offset of text flow in pixels
			int oldTextFlowYOffset = calculateTextFlowYOffset();

			// How much is gazeY away from elements center used for speed
			float offsetSpeed = ((float)(4 * (flowY - (mHeight / 2))) / (float)mHeight); // [-0.5, 0.5]

			// Normalize speed by height of text flow
			float textFlowHeight = (mupTextFlow->getHeight() + mupTextFlow->getLineHeight()); // add line height to avoid cutting letters like 'p'
			offsetSpeed /= textFlowHeight / (float)mHeight;

			// Update relative offset
			mTextFlowYOffset.update(offsetSpeed * tpf * mpLayout->getConfig()->textEditScrollSpeedMultiplier);

			// *** ACTIVE WORD ***

			// Get currently selected word
			TextFlow::FlowWord newActiveWord;

			// Decide whether active word has changed
			if (mupTextFlow->getFlowWord(flowX, flowY + oldTextFlowYOffset, newActiveWord))
			{
				if (mupActiveWord != NULL)
				{
					// Compare flow word index
					if (newActiveWord.index != mupActiveWord->index)
					{
						// Flow word index is different, use it!
						setActiveWord(newActiveWord, true);
					}
				}
				else
				{
					// No active word was set, do it now
					setActiveWord(newActiveWord, true);
				}
			}
		}

        return adaptiveScale;
    }

    void TextEdit::specialDraw() const
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

		// *** TEXT ***

		// Push scissor to render text only within element
		pushScissor(mX, mY, mWidth, mHeight);

		// Y offset of text flow in pixels
		int textFlowYOffset = calculateTextFlowYOffset();

		// Function for draw matrix of active word background
		std::function<glm::mat4(int, int, int)> calculateActiveWordBackgroundDrawMatrix = [&](int x, int y, int width)
		{
			// Calculate width and height for active word background
			int activeBackgroundWidth = (int)(TEXT_EDIT_ACTIVE_WORD_BACKGROUND_SIZE * (float)width);
			int activeBackgroundHeight = (int)(TEXT_EDIT_ACTIVE_WORD_BACKGROUND_SIZE * mupTextFlow->getLineHeight());

			// Calculate draw matrix
			return calculateDrawMatrix(
				mpLayout->getLayoutWidth(),
				mpLayout->getLayoutHeight(),
				mX + x - ((activeBackgroundWidth - width) / 2),
				mY + y - textFlowYOffset - ((activeBackgroundHeight - (int)mupTextFlow->getLineHeight()) / 2),
				activeBackgroundWidth,
				activeBackgroundHeight);
		};

		// Draw background behind active word (or better said behind active sub words
		mpActiveWordBackground->bind();
		mpActiveWordBackground->getShader()->fillValue("color", getStyle()->markColor); // TODO: marked color used. Maybe use some custom

		// Draw previous active ones
		for (const auto& rPair : mPreviousActiveWords)
		{
			for (const auto& rSubFlowWord : rPair.first)
			{
				// Calculate draw matrix
				glm::mat4 activeWordBackgroundDrawMatrix = calculateActiveWordBackgroundDrawMatrix(rSubFlowWord.x, rSubFlowWord.y, rSubFlowWord.width);

				// Draw previous active sub word's background
				mpActiveWordBackground->getShader()->fillValue("matrix", activeWordBackgroundDrawMatrix);
				mpActiveWordBackground->getShader()->fillValue("alpha", (rPair.second / mpLayout->getConfig()->animationDuration) * getMultipliedDimmedAlpha());
				mpActiveWordBackground->draw();
			}
		}

		// Draw currently active one
		if (mupActiveWord != NULL)
		{
			for (const auto& rSubFlowWord : mupActiveWord->subWords)
			{
				// Calculate draw matrix
				glm::mat4 activeWordBackgroundDrawMatrix = calculateActiveWordBackgroundDrawMatrix(rSubFlowWord.x, rSubFlowWord.y, rSubFlowWord.width);

				// Draw active sub word's background
				mpActiveWordBackground->getShader()->fillValue("matrix", activeWordBackgroundDrawMatrix);
				mpActiveWordBackground->getShader()->fillValue("alpha", (mActiveWordFading / mpLayout->getConfig()->animationDuration) * getMultipliedDimmedAlpha());

				mpActiveWordBackground->draw();
			}
		}

		// Drawing of text flow
		mupTextFlow->draw(
			getStyle()->fontColor,
			mAlpha,
			false,
			0,
			textFlowYOffset);

		// Calculate x and y of cursor
		int cursorX = 0;
		int cursorY = 0;

		// If there is a active word, get cursor position
		if (mupActiveWord != NULL)
		{
			int letterOffsetX = 0;
			if (mCursorLetterIndex >= 0)
			{
				letterOffsetX = mupActiveWord->subWords.at(mCursorSubWordIndex).lettersXOffsets.at(mCursorLetterIndex);
			}

			cursorX = mupActiveWord->subWords.at(mCursorSubWordIndex).x
				+ letterOffsetX;
			cursorY = mupActiveWord->subWords.at(mCursorSubWordIndex).y;
		}

		// Calculate matrix for cursor
		glm::mat4 cursorDrawMatrix = calculateDrawMatrix(
			mpLayout->getLayoutWidth(),
			mpLayout->getLayoutHeight(),
			mX + cursorX,
			mY + cursorY - textFlowYOffset,
			glm::max(1, (int)(TEXT_EDIT_CURSOR_RELATIVE_WIDTH * mupTextFlow->getPixelWidthOfSpace())),
			(int)mupTextFlow->getLineHeight());

		// Draw pulsing cursor over text
		mpCursor->bind();
		mpCursor->getShader()->fillValue("matrix", cursorDrawMatrix);
		mpCursor->getShader()->fillValue("color", getStyle()->fontColor);
		mpCursor->getShader()->fillValue("alpha", getMultipliedDimmedAlpha() * (glm::cos(mCursorPulse) * 0.5f) + 0.5f);
		mpCursor->draw();

		// Pop own scissor
		popScissor();

        // Draw stuff like highlighting
        InteractiveElement::specialDraw();
    }

    void TextEdit::specialTransformAndSize()
    {
		// Tell text flow about transformation
		mupTextFlow->transformAndSize(mX, mY, mWidth, mHeight);

		// Unset active word
		mupActiveWord.reset();
    }

    void TextEdit::specialReset()
    {
        // Call super
		InteractiveElement::specialReset();

		// Class resets
		mCursorPulse = 1.f;
    }

    void TextEdit::specialInteract()
    {
		// TODO
    }

    void TextEdit::specialPipeNotification(NotificationType notification, Layout* pLayout)
    {
        // Pipe notifications to notifier template including own data
        switch (notification)
        {
        default:
            throwWarning(
                OperationNotifier::Operation::BUG,
                "Text edit got notification which is not thought for it.");
            break;
        }
    }

	int TextEdit::calculateTextFlowYOffset() const
	{
		return (int)(mTextFlowYOffset.getValue() * glm::max(0.f, (float)((mupTextFlow->getHeight() + mupTextFlow->getLineHeight()) - mHeight)));
	}

	void TextEdit::setActiveWord(const TextFlow::FlowWord& rFlowWord, bool setCursorToEnd)
	{
		if (mupActiveWord != NULL)
		{
			mPreviousActiveWords.push_back(
				std::make_pair(
					mupActiveWord->subWords,
					mActiveWordFading));
		}

		// Save active word in memmber
		mupActiveWord = std::unique_ptr<TextFlow::FlowWord>(new TextFlow::FlowWord(rFlowWord));
		mActiveWordFading = 0.f;

		// Set cursor
		if (setCursorToEnd)
		{
			// Set cursor position to last subword of active word
			mCursorSubWordIndex = (int)mupActiveWord->subWords.size() - 1;

			// Set cursor position to last letter in subword
			mCursorLetterIndex = (int)mupActiveWord->subWords.at(mCursorSubWordIndex).lettersXOffsets.size() - 1;
		}

		// Reset cursor pulse to make it directly visible
		mCursorPulse = 0.f;
	}
}
