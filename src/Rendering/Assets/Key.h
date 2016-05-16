//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keys for keyboard element. Created by asset manager.

#ifndef KEY_H_
#define KEY_H_

#include "src/Rendering/Font/Font.h"
#include "src/Rendering/Shader.h"
#include "src/Rendering/RenderItem.h"
#include "src/Utilities/LerpValue.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    // Forward declaration
    class Layout;
    class AssetManager;

    // Abstract superclass of key
    class Key
    {
    public:

        // Constructor
        Key(Layout const * pLayout, AssetManager* pAssetManager);

        // Copy constructor
        Key(const Key& rOtherKey);

        // Destructor
        virtual ~Key() = 0;

        // Simple transform and size to fit layout size
        virtual void transformAndSize();

        // Set position of center and size of key
        virtual void transformAndSize(int x, int y, int size);

        // Update
        virtual void update(float tpf);

        // Draw
		virtual void draw(
			glm::vec4 color,
			glm::vec4 pickColor,
			glm::vec4 iconColor,
			glm::vec4 thresholdColor,
			float threshold,
			float alpha) const = 0;

        // Reset
        virtual void reset();

        // Set focus
        void setFocus(bool doFocus);

        // Get focus
        bool isFocused() const;

        // Get value of focus
        float getFocusValue() const;

        // Picked
        void setPicked(bool picked);

        // Get picked
        bool isPicked() const;

        // Get position on GUI
        glm::vec2 getPosition() const;

        // Get size in pixels
        int getSize() const;

        // Get value
        virtual std::u16string getValue() const = 0;

    protected:

        // Draw circle
        void drawCircle(
            glm::vec4 color,
            glm::vec4 pickColor,
            float alpha) const;

		// Draw threshold
		void drawThreshold(
			glm::vec4 thresholdColor,
			float threshold,
			float alpha) const;

        // Members
        int mX;
        int mY;
        int mSize;
        Layout const * mpLayout;
        AssetManager* mpAssetManager;
        RenderItem const * mpCircleRenderItem;
		RenderItem const * mpThresholdItem;
        bool mFocused;
        LerpValue mFocus;
        bool mPicked;
        LerpValue mPick;
        glm::mat4 mCircleMatrix;
    };
}

#endif // KEY_H_
