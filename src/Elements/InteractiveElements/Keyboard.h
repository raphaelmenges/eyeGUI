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
#include "src/Utilities/LerpValue.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

namespace eyegui
{
    class Keyboard : public InteractiveElement, public NotifierTemplate<KeyboardListener>
    {
    public:

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

        // Set fast typing
        void setFastTyping(bool useFastTyping);

        // Set whether small or big characters are used
        void setCase(KeyboardCase keyboardCase);

        // Get count of keymaps
        uint getCountOfKeymaps() const;

        // Set displayed keymap by index
        void setKeymap(uint keymapIndex);

        // Tries to fetch next interactive element for selecting, returns NULL if fails
        virtual InteractiveElement* internalNextInteractiveElement(Element const * pChildCaller);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filled by subclasses
        virtual void specialReset();

        // Interaction fill by subclasses
        virtual void specialInteract();

        // Filled by subclass and called by layout after updating and before drawing
        virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

    private:

        // Typedefs
        typedef std::pair<float, std::unique_ptr<Key> > PressedKey; // Alpha and copy of key
        typedef std::vector<std::vector<std::unique_ptr<Key> > > SubKeymap; // Sub keymap (only used to hold either small or big versions of characters
        typedef std::vector<std::vector<glm::vec2> > PositionMap; // Positions of keys in keymap

        // Struct for keymap which holds sub keymaps of small and big letters
        struct Keymap
        {
            SubKeymap smallKeys;
            SubKeymap bigKeys;
            PositionMap initialKeyPositions;
            float initialKeySize;
        };

        // Struct for pairs of small and big characters used to initialize the keyboard
        struct CPair
        {
            CPair(char16_t small, char16_t big)
            {
                this->small = small;
                this->big = big;
            }

            CPair(char16_t character)
            {
                this->small = character;
                this->big = character;
            }

            char16_t small;
            char16_t big;
        };

        // Keymap creation methods
        void initKeymaps(CharacterSet set);

        // Add many keys at once
        void addKeys(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions, const std::vector<std::vector<CPair> >& input);

        // Add key to sub keymaps
        void addKey(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions, char16_t smallCharacter, char16_t bigCharacter) const;

        // Add new line to sub keymaps
        void newLine(SubKeymap& rSmallKeys, SubKeymap& rBigKeys, PositionMap& rInitialPositions) const;

        // Reset keymaps
        void resetKeymapsAndState();

		// Press key
		void pressKey(SubKeymap* pKeys, int i, int j);

        // Members
        RenderItem const * mpBackground;
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
        std::vector<Keymap> mKeymaps;
        uint mCurrentKeymapIndex;
        bool mBigCharactersActive;
        int mLastFastKeyRow;
        int mLastFastKeyColumn;
    };
}

#endif // KEYBOARD_H_
