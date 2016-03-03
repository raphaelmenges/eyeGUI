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
        mSelected = false;
        mSelection.setValue(0);

        // Fetch render item for key circle
        mpCircleRenderItem = mpAssetManager->fetchRenderItem(
            shaders::Type::KEY,
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
        mSelected = rOtherKey.mSelected;
        mSelection.setValue(rOtherKey.mSelection.getValue());
        mpCircleRenderItem = rOtherKey.mpCircleRenderItem;
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
        mSelection.update(tpf / KEY_SELECT_DURATION, !mSelected);
    }

    void Key::reset()
    {
        mFocused = false;
        mFocus.setValue(0);
        mSelected = false;
        mSelection.setValue(0);
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

    void Key::setSelect(bool doSelect)
    {
        mSelected = doSelect;
    }

    bool Key::isSelected() const
    {
        return mSelected;
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
            int oglStencilX,
            int oglSencilY,
            int oglStencilWidth,
            int oglStencilHeight,
            glm::vec4 color,
            glm::vec4 selectionColor,
            float activity,
            glm::vec4 dimColor,
            float dim,
            glm::vec4 markColor,
            float mark,
            glm::vec4 highlightColor,
            float highlight,
            float alpha) const
    {
        // Bind and fill render item
        mpCircleRenderItem->bind();

        // Fill color
        glm::vec4 circleColor = color;
        circleColor.a *= alpha;
        mpCircleRenderItem->getShader()->fillValue("color", circleColor);

        // Fill other uniforms
        mpCircleRenderItem->getShader()->fillValue("time", mpLayout->getAccPeriodicTime());
        mpCircleRenderItem->getShader()->fillValue("matrix", mCircleMatrix); // Matrix is updated in transform and size
        mpCircleRenderItem->getShader()->fillValue("selection", mSelection.getValue());
        mpCircleRenderItem->getShader()->fillValue("selectionColor", selectionColor);
        mpCircleRenderItem->getShader()->fillValue("stencil", glm::vec4(oglStencilX, oglSencilY, oglStencilWidth, oglStencilHeight));
        mpCircleRenderItem->getShader()->fillValue("activity", activity);
        mpCircleRenderItem->getShader()->fillValue("dimColor", dimColor);
        mpCircleRenderItem->getShader()->fillValue("dim", dim);
        mpCircleRenderItem->getShader()->fillValue("markColor", markColor);
        mpCircleRenderItem->getShader()->fillValue("mark", mark);
        mpCircleRenderItem->getShader()->fillValue("highlightColor", highlightColor);
        mpCircleRenderItem->getShader()->fillValue("highlight", highlight);

        // Drawing
        mpCircleRenderItem->draw();
    }
}
