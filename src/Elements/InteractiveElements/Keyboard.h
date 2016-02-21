//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input. Let asset manager create the keys.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"
#include "src/Elements/NotifierTemplate.h"
#include "externals/GLM/glm/glm.hpp"
#include "src/LerpValue.h"

#include <vector>

namespace eyegui
{
    class Keyboard : public InteractiveElement, public NotifierTemplate<KeyboardListener>
    {
    public:

        // TODO
		// - make fast typing more robust (maybe make threshold increase slower)
		// - What happens, when no key is selected?
		// - Use selected key for interact?
		// - implement InteractiveElement stuff (and elements stuff)
		//	- activity
		//  - dim
		//  - mark
		//	- highlight
		//  - selection

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

		// Interaction fill by subclasses
		virtual void specialInteract();

        // Filled by subclass and called by layout after updating and before drawing
        virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

    private:

		// Typedefs
		typedef std::pair<float, std::unique_ptr<Key> > PressedKey;

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
        glm::vec2 mGazePosition;
        std::u16string mLastPressedKeyValue;
		bool mKeyWasPressed;
		std::vector<PressedKey> mPressedKeys;  // Alpha [0..1] and copy of key
		bool mUseFastTyping;
		std::u16string mFastBuffer;
    };
}

#endif // KEYBOARD_H_
