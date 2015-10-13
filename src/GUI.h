//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// GUI class owning the layouts.

#ifndef GUI_H_
#define GUI_H_

#include "eyeGUI.h"

#include "Layout.h"
#include "Parser/LayoutParser.h"
#include "Parser/ConfigParser.h"
#include "Rendering/AssetManager.h"
#include "Rendering/GLSetup.h"
#include "Input.h"
#include "Config.h"

#include <memory>
#include <vector>

namespace eyegui
{
    class GUI
    {
    public:

        // Local constants
        const std::string NO_CONFIG_TO_LOAD = "";

        // *** Methods accessed by other classes ***

        // Getter for window size
        int getWindowWidth() const;
        int getWindowHeight() const;

        // Get time since start (gets reset if too big)
        float getAccPeriodicTime() const;

        // Get pointer to config of this GUI
        Config const * getConfig() const;

        // Get used character set
        CharacterSet getCharacterSet() const;

        // Get set default font
        Font const * getDefaultFont() const;

        // *** Methods accessed via interface ***

        // Constructor
        GUI(
            int width,
            int height,
            std::string fontFilepath,
            CharacterSet characterSet);

        // Destructor
        virtual ~GUI();

        // Load layout from xml, returns NULL if fails
        Layout* addLayout(std::string filepath, bool visible);

        // Resize whole gui
        void resize(int width, int height);

        // Render all layouts
        void render(float tpf);

        // Set mouse cursor position
        void setMouseCursor(int x, int y);

        // Move layout to front
        void moveLayoutToFront(Layout* pLayout);

        // Move layout to back
        void moveLayoutToBack(Layout* pLayout);

        // Load a config
        void loadConfig(std::string filepath);

        // Prefatch image to avoid lag
        void prefetchImage(std::string filepath);

    private:

        // Find index of layout, returns -1 if fails
        int findLayout(Layout const * pLayout) const;

        // Move layout
        void moveLayout(int oldIndex, int newIndex);

        // Members
        std::vector<std::unique_ptr<Layout> > mLayouts;
        int mWidth, mHeight;
        CharacterSet mCharacterSet;
        LayoutParser mLayoutParser;
        ConfigParser mConfigParser;
        std::unique_ptr<AssetManager> mupAssetManager;
        GLSetup mGLSetup;
        Input mInput;
        float mAccPeriodicTime;
        Config mConfig;
        bool mLayoutsLocked;
        std::string mConfigToLoad;
        Font const * mpDefaultFont;
    };
}

#endif // GUI_H_
