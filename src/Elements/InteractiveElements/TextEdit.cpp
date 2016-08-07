//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "TextEdit.h"

#include "src/Layout.h"
#include "src/Rendering/ScissorStack.h"
#include "src/Utilities/Helper.h"

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
		mCursorPulse = 1.f;

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

    float TextEdit::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

		// Scroll text flow
		if (penetratedByInput(pInput) && (mupTextFlow->getHeight() > 0))
		{
			// Y in element coordinates
			int y = pInput->gazeY - mY;

			// How much is gazeY away from elements center used for speed
			float offsetSpeed = ((float)(4 * (y - (mHeight / 2))) / (float)mHeight); // [-0.5, 0.5]

			// Normalize speed by height of text flow
			float textFlowHeight = (mupTextFlow->getHeight() + mupTextFlow->getLineHeight()); // add line height to avoid cutting letters like 'p'
			offsetSpeed /= textFlowHeight / (float)mHeight;

			// Update relative offset
			mTextFlowYOffset.update(offsetSpeed * tpf * mpLayout->getConfig()->textEditScrollSpeedMultiplier);
		}

		// Update pulsing of cursor
		float fullCircle = 2 * glm::pi<float>();
		mCursorPulse += (tpf * fullCircle) / TEXT_EDIT_CURSOR_PULSE_DURATION;
		while (mCursorPulse >= fullCircle) { mCursorPulse -= fullCircle;  }

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

		// Y offset of text flow
		int textFlowYOffset = (int)(mTextFlowYOffset.getValue() * glm::max(0.f, (float)(mupTextFlow->getHeight() - mHeight)));

		// Draw background behind active word (or better said behind active sub words
		mpActiveWordBackground->bind();
		mpActiveWordBackground->getShader()->fillValue("color", glm::vec4(1,0,0,1));
		mpActiveWordBackground->getShader()->fillValue("alpha", getMultipliedDimmedAlpha());
		for (const auto& rSubFlowWord : mupTextFlow->getSubFlowWord(5)) // TODO test
		{
			// Calculate draw matrix
			glm::mat4 activeWordBackgroundDrawMatrix = calculateDrawMatrix(
				mpLayout->getLayoutWidth(),
				mpLayout->getLayoutHeight(),
				mX + rSubFlowWord.x,
				mY + rSubFlowWord.y - textFlowYOffset,
				rSubFlowWord.width,
				(int)mupTextFlow->getLineHeight());

			// Draw active sub word's background
			mpActiveWordBackground->getShader()->fillValue("matrix", activeWordBackgroundDrawMatrix);
			mpActiveWordBackground->draw();
		}

		// Drawing of text flow
		mupTextFlow->draw(
			getStyle()->fontColor,
			mAlpha,
			false,
			0,
			textFlowYOffset);

		// Calculate matrix for cursor
		glm::mat4 cursorDrawMatrix = calculateDrawMatrix(
			mpLayout->getLayoutWidth(),
			mpLayout->getLayoutHeight(),
			mX,
			mY - textFlowYOffset,
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
}
