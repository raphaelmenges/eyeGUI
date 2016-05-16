//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Key.h"

#include "src/Layout.h"
#include "src/Rendering/AssetManager.h"
#include "src/Defines.h"
#include "src/Utilities/Helper.h"

namespace eyegui
{
    Key::Key(Layout const * pLayout, AssetManager* pAssetManager)
    {
        // Initialize members
        mpLayout = pLayout;
        mpAssetManager = pAssetManager;

        // TransformAndSize has to be called before usage
        mX = 0;
        mY = 0;
        mSize = 0;
        mFocused = false;
        mFocus.setValue(0);
        mPicked = false;
        mPick.setValue(0);

        // Fetch render item for key circle
        mpCircleRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
            meshes::Type::QUAD);

		// Fetch render item for threshold visualization
		mpThresholdItem = mpAssetManager->fetchRenderItem(
			shaders::Type::CIRCLE_THRESHOLD,
			meshes::Type::QUAD);
    }

    Key::Key(const Key& rOtherKey)
    {
        // Copy members
        mpLayout = rOtherKey.mpLayout;
        mpAssetManager = rOtherKey.mpAssetManager;
        mX = rOtherKey.mX;
        mY = rOtherKey.mY;
        mSize = rOtherKey.mSize;
        mFocused = rOtherKey.mFocused;
        mFocus.setValue(rOtherKey.mFocus.getValue());
        mPicked = rOtherKey.mPicked;
        mPick.setValue(rOtherKey.mPick.getValue());
        mpCircleRenderItem = rOtherKey.mpCircleRenderItem;
		mpThresholdItem = rOtherKey.mpThresholdItem;
    }

    Key::~Key()
    {
        // Nothing to do
    }

    void Key::transformAndSize()
    {
        // Draw matrix for cirlce
        mCircleMatrix = calculateDrawMatrix(
            mpLayout->getLayoutWidth(),
            mpLayout->getLayoutHeight(),
            mX - mSize / 2,
            mY - mSize / 2,
            mSize,
            mSize);
    }

    void Key::transformAndSize(int x, int y, int size)
    {
        mX = x;
        mY = y;
        mSize = size;

        transformAndSize();
    }

    void Key::update(float tpf)
    {
        mFocus.update(tpf / KEY_FOCUS_DURATION, !mFocused);
        mPick.update(tpf / KEY_SELECT_DURATION, !mPicked);
    }

    void Key::reset()
    {
        mFocused = false;
        mFocus.setValue(0);
        mPicked = false;
        mPick.setValue(0);
    }

    void Key::setFocus(bool doFocus)
    {
        mFocused = doFocus;
    }

    bool Key::isFocused() const
    {
        return mFocused;
    }

    float Key::getFocusValue() const
    {
        return mFocus.getValue();
    }

    void Key::setPicked(bool picked)
    {
        mPicked = picked;
    }

    bool Key::isPicked() const
    {
        return mPicked;
    }

    glm::vec2 Key::getPosition() const
    {
        return glm::vec2(mX, mY);
    }

    int Key::getSize() const
    {
        return mSize;
    }

    void Key::drawCircle(
            glm::vec4 color,
            glm::vec4 pickColor,
            float alpha) const
    {
        // Bind and fill render item
        mpCircleRenderItem->bind();

        // Fill color
        glm::vec4 circleColor = color;
        circleColor.a *= alpha;
        mpCircleRenderItem->getShader()->fillValue("color", circleColor);

        // Fill other uniforms
        mpCircleRenderItem->getShader()->fillValue("matrix", mCircleMatrix); // Matrix is updated in transform and size
        mpCircleRenderItem->getShader()->fillValue("pickColor", pickColor);
        mpCircleRenderItem->getShader()->fillValue("pick", mPick.getValue());

        // Drawing
        mpCircleRenderItem->draw();
    }

	void Key::drawThreshold(
		glm::vec4 thresholdColor,
		float threshold,
		float alpha) const
	{
		if (threshold > 0)
		{
			mpThresholdItem->bind();
			mpThresholdItem->getShader()->fillValue("matrix", mCircleMatrix);
			mpThresholdItem->getShader()->fillValue("thresholdColor", thresholdColor);
			mpThresholdItem->getShader()->fillValue("threshold", threshold);
			mpThresholdItem->getShader()->fillValue("alpha", alpha);
			mpThresholdItem->getShader()->fillValue("mask", 0); // mask is always in slot 0
			mpThresholdItem->draw();
		}
	}
}
