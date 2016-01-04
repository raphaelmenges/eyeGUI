//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Keyboard.h"

namespace eyegui
{
	Keyboard::Keyboard(
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
		bool adaptiveScaling) : Element(
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
		mType = Type::KEYBOARD;

		// Fetch render item for background
		mpBackground = mpAssetManager->fetchRenderItem(
			shaders::Type::BLOCK,
			meshes::Type::QUAD);
	}

	Keyboard::~Keyboard()
	{
		// Nothing to do
	}

	float Keyboard::specialUpdate(float tpf, Input* pInput)
	{
		return 0;
	}

	void Keyboard::specialDraw() const
	{
		if (getStyle()->backgroundColor.a > 0)
		{
			// Bind render item before setting values and drawing
			mpBackground->bind();

			// Fill matrix in shader
			mpBackground->getShader()->fillValue("matrix", mDrawMatrix);

			// Fill color to shader
			mpBackground->getShader()->fillValue(
				"backgroundColor",
				getStyle()->backgroundColor);

			// Fill alpha
			mpBackground->getShader()->fillValue("alpha", mAlpha);

			// Fill activity
			mpBackground->getShader()->fillValue("activity", mActivity.getValue());

			// Fill dimming
			mpBackground->getShader()->fillValue("dimColor", getStyle()->dimColor);
			mpBackground->getShader()->fillValue("dim", mDim.getValue());

			// Fill marking
			mpBackground->getShader()->fillValue("markColor", getStyle()->markColor);
			mpBackground->getShader()->fillValue("mark", mMark.getValue());

			// Draw render item
			mpBackground->draw();
		}
	}

	void Keyboard::specialTransformAndSize()
	{
		// Nothing to do
	}

	void Keyboard::specialReset()
	{
		// Nothing to do
	}

	bool Keyboard::mayConsumeInput()
	{
		return false;
	}
}
