//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Keyboard for eyetracking input.

#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "Element.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

namespace eyegui
{
    class Keyboard : public Element
    {
    public:

        // TODO
        //

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

        // Inner class for keys
        class Key
        {
        public:

            // Constructor
            Key(Layout const * pLayout, AssetManager* pAssetManager);

            // Set position of center and size of key
            void transformAndSize(int x, int y, int size);

            // Draw key
            virtual void draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const = 0;

        protected:

            // Members
            int mX;
            int mY;
            int mSize;
            RenderItem const * mpRenderItem;
            Layout const * mpLayout;
        };

        // Key with font letter
        class FontKey : public Key
        {
        public:

            // Constructor
            FontKey(
                Layout const * pLayout,
                AssetManager* pAssetManager,
                Font const * pFont,
                char16_t character);

            // Draw key
            virtual void draw(glm::vec4 color, glm::vec4 iconColor, float alpha) const;

        private:

            // Members
            char16_t mCharacter;
            Font const * mpFont;
        };

        // Key with graphics
        class GraphicsKey : public Key
        {
        public:

            // Constructor
            GraphicsKey(
                Layout const * pLayout,
                AssetManager* pAssetManager);
        };

        // Add key to keyboard
        void addKey(char16_t character);

        // Start new line for keyboard
        void newLine();

        // Members
        Font const * mpFont;
        RenderItem const * mpBackground;
        std::vector<std::vector<std::unique_ptr<Key> > > mKeys;
    };
}

#endif // KEYBOARD_H_
