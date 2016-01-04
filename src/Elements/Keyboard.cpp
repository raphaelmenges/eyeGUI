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

		// Fetch render item for keys
		mpKey = mpAssetManager->fetchRenderItem(
			shaders::Type::KEY,
			meshes::Type::QUAD);

		// TODO: Testing
		mKeyCount = 25;
	}

	Keyboard::~Keyboard()
	{
		// Nothing to do
	}

	float Keyboard::specialUpdate(float tpf, Input* pInput)
	{
		// TODO: update keys (maybe do not save keycount but vector with matrices and...vector with letters...)
		return 0;
	}

	void Keyboard::specialDraw() const
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

		// *** KEYS ***
		mpKey->bind();
		mpKey->getShader()->fillValue("alpha", mAlpha);

		// !!! THINK AND THEN CODE... !!!

		// Calculate size of one key and count of rows (TODO: move to update method)
		int size;
		unsigned int rowCount = 1;

		// First case: one row
		size = mWidth / mKeyCount;
		if (size >= mHeight)
		{
			// Only one row
			size = mHeight;
		}
		else
		{
			// Start computation of row count and size
			int newSize = size;
			while (newSize >= size)
			{
				rowCount++;
				unsigned int virtualKeyCount = mKeyCount + (rowCount / 2); // Add virtual keys for the shifted appereance
				virtualKeyCount / rowCount;
			}
		}

		// Calculate position of keys

		for (unsigned int i = 0; i < mKeyCount; i++)
		{
			/*glm::mat4 matrix = calculateDrawMatrix((int)(mX + i*size), mY, (int)size, (int)size);
			mpKey->getShader()->fillValue("matrix", matrix);
			mpKey->draw();*/
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
