//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "FutureKey.h"

#include "src/Rendering/AssetManager.h"
#include "src/Layout.h"
#include "src/Utilities/Helper.h"
#include "src/Rendering/ScissorStack.h"

namespace eyegui
{
	FutureKey::FutureKey(Layout const * pLayout, AssetManager* pAssetManager)
	{
		// Initialize members
		mX = 0;
		mY = 0;
		mWidth = 0;
		mHeight = 0;

		// Save members
		mpLayout = pLayout;
		mpAssetManager = pAssetManager;

		// Fetch render items
		mpThresholdItem = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE_THRESHOLD, meshes::Type::QUAD);
	}

	FutureKey::~FutureKey()
	{

	}

	void FutureKey::transformAndSize(int x, int y, int width, int height)
	{
		mX = x;
		mY = y;
		mWidth = width;
		mHeight = height;
	}

	void FutureKey::update(float tpf)
	{

	}

	void FutureKey::draw(float alpha) const
	{
		// Push scissor to limit rendering to current key (especially threshold)
		pushScissor(mX, mY, mWidth, mHeight);

		// Calculate draw matrix for threshold
		int thresholdWidth = mWidth * 2;
		int thresholdHeight = mHeight * 3;
		int thresholdX = mX;
		int thresholdY = mX;
		glm::mat4 thresholdDrawMatrix = calculateDrawMatrix(
			mpLayout->getLayoutWidth(),
			mpLayout->getLayoutHeight(),
			thresholdX,
			thresholdY,
			thresholdWidth,
			thresholdHeight);

		// Draw threshold
		mpThresholdItem->bind();
		mpThresholdItem->getShader()->fillValue("matrix", thresholdDrawMatrix);
		mpThresholdItem->getShader()->fillValue("thresholdColor", glm::vec4(1,0,0,1));
		mpThresholdItem->getShader()->fillValue("threshold", 0.5f);
		mpThresholdItem->getShader()->fillValue("alpha", alpha);
		mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
		mpThresholdItem->draw();

		// Pop scissor again
		popScissor();
	}

	void FutureKey::reset()
	{

	}
}