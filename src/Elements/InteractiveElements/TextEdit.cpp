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

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::COLOR,
			meshes::Type::QUAD);

		// Create text flow
		mupTextFlow = std::move(mpAssetManager->createTextFlow(fontSize, TextFlowAlignment::LEFT, TextFlowVerticalAlignment::TOP, 1.f, u"TextEdit"));
	}

    TextEdit::~TextEdit()
    {
        // Nothing to do
    }

    float TextEdit::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

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

		// Drawing of text flow
		mupTextFlow->draw(getStyle()->fontColor, mAlpha);

		// TODO: Scissor stack usage

        // Draw stuff like highlighting
        InteractiveElement::specialDraw();
    }

    void TextEdit::specialTransformAndSize()
    {
		// Tell text flow about transformation
		mupTextFlow->transformAndSize(mX, mY, mWidth, mHeight);

		// TODO: textflow may be bigger than element, depending on how much height is necessary
    }

    void TextEdit::specialReset()
    {
        // Call super
		InteractiveElement::specialReset();
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
