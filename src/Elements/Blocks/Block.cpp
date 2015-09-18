//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Block.h"

namespace eyegui
{
	Block::Block(
		std::string id,
		std::string styleName,
		Element* pParent,
		Layout* pLayout,
		AssetManager* pAssetManager,
		float relativeScale,
		float border) : Element(
			id,
			styleName,
			pParent,
			pLayout,
			pAssetManager,
			relativeScale,
			border)
	{
		mType = Type::BLOCK;

		// Fetch render item
		mpBackground = pAssetManager->fetchRenderItem(
			shaders::Type::BLOCK,
			meshes::Type::QUAD);
	}

	Block::~Block()
	{
		// Nothing to do so far
	}

	// Updating
	void Block::specialUpdate(float tpf, Input* pInput)
	{
		// If mouse over block, consume input (copied from INTERACTIVE element)
		// TODO: Using the alpha is somehow..strange
		if (pInput != NULL
			&& !pInput->mouseUsed
			&& getStyle()->backgroundColor.a > 0)
		{
			if (pInput->mouseCursorX >= mX
				&& pInput->mouseCursorX <= mX + mWidth
				&& pInput->mouseCursorY >= mY
				&& pInput->mouseCursorY <= mY + mHeight)
			{
				pInput->mouseUsed = true;
			}
		}
	}

	void Block::specialDraw() const
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
			mpBackground->getShader()->fillValue("activity", mActivity);

			// Draw render item
			mpBackground->draw();
		}
	}

	void Block::specialTransformAndSize()
	{
		// Nothing to do
	}

	void Block::specialReset()
	{
		// Nothing to do
	}
}
