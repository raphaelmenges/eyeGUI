//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input. Let asset manager create the keys.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Element.h"
#include "externals/GLM/glm/glm.hpp"
#include "LerpValue.h"

#include <vector>

namespace eyegui
{
    class Keyboard : public Element
    {
    public:

    // TODO
    // - Give focus real color and more speed
    // Make modes, which can be activated / deactivated
    // Or more like parameters, which can adjust it?
    // - grow mode
    // - fast mode

    // Notes
    // - icon color is used here for font rendering


        // Constructor
        Keyboard(
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
            bool adaptiveScaling);

        // Destructor
        virtual ~Keyboard();

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filled by subclasses
        virtual void specialReset();

        // Implemented by subclasses
        virtual bool mayConsumeInput();

    private:

        // Add key to keyboard
        void addKey(char16_t character);

        // Start new line for keyboard
        void newLine();

        // Members
        RenderItem const * mpBackground;
        std::vector<std::vector<std::unique_ptr<Key> > > mKeys;
        std::vector<std::vector<glm::vec2> > mInitialKeyPositions;
        float mInitialKeySize;
        LerpValue mThreshold;
        int mFocusedKeyRow;
        int mFocusedKeyColumn;
        glm::vec2 mFocusPosition;
        bool mNewFocus;
    };
}

#endif // KEYBOARD_H_
