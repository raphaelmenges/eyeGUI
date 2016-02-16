//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// GUI class owning the layouts. Most access through interface is handled
// using special job objects, which are executed before rendering. This
// ensures that the vector of layouts is not changed during rendering and
// notifications from elements can trigger GUI jobs.

#ifndef GUI_H_
#define GUI_H_

#include "include/eyeGUI.h"

#include "Layout.h"
#include "Parser/LayoutParser.h"
#include "Parser/ConfigParser.h"
#include "Parser/LocalizationParser.h"
#include "Rendering/AssetManager.h"
#include "Rendering/GLSetup.h"
#include "Config.h"
#include "Rendering/GazeDrawer.h"

#include <memory>
#include <vector>

namespace eyegui
{
    class GUI
    {
    public:

        // *** Methods accessed via interface ***

        // Constructor
        GUI(
            int width,
            int height,
            std::string fontFilepath,
            CharacterSet characterSet,
            std::string localizationFilepath,
            float vectorGraphicsDPI,
            float fontTallSize,
            float fontMediumSize,
            float fontSmallSize);

        // Destructor
        virtual ~GUI();

        // Load layout from xml, returns NULL if fails
        Layout* addLayout(std::string filepath, bool visible);

        // Remove layout
        void removeLayout(Layout const * pLayout);

        // Resize whole gui
        void resize(int width, int height);

        // Updating
        Input update(float tpf, const Input input);

        // Drawing
        void draw() const;

        // Move layout to front
        void moveLayoutToFront(Layout* pLayout);

        // Move layout to back
        void moveLayoutToBack(Layout* pLayout);

        // Load a config
        void loadConfig(std::string filepath);

        // Set gaze visualization drawing
        void setGazeVisualizationDrawing(bool draw);

        // Toggle gaze visualization drawing
        void toggleGazeVisualizationDrawing();

        // Prefatch image to avoid lag
        void prefetchImage(std::string filepath);

        // Set value of config attribute
        void setValueOfConfigAttribute(std::string attribute, std::string value);

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

        // Get string content from localization
        std::u16string getContentFromLocalization(std::string key) const;

        // Get dpi for rasterization of vector graphics
        float getVectorGraphicsDPI() const;

        // Get size for tall font
        float getFontTallSize() const;

        // Get size for medium font
        float getFontMediumSize() const;

        // Get size for small font
        float getFontSmallSize() const;

    private:

        // ### INNER CLASSES ###################################################

        // Job for the gui to execute before rendering
        class GUIJob
        {
        public:

            GUIJob(GUI* pGUI);
            virtual void execute() = 0;

        protected:

            GUI* mpGUI;
        };

        // Job to move layout
        class MoveLayoutJob : public GUIJob
        {
        public:

            MoveLayoutJob(GUI* pGUI, Layout* pLayout, bool toFront);
            virtual void execute();

        protected:

            Layout* mpLayout;
            bool mToFront; // otherwise to back
        };

        // Job to load config
        class LoadConfigJob : public GUIJob
        {
        public:

            LoadConfigJob(GUI* pGUI, std::string filepath);
            virtual void execute();

        protected:

            std::string mFilepath;
        };

        // Job to add layout
        class AddLayoutJob : public GUIJob
        {
        public:

            AddLayoutJob(GUI* pGUI, std::unique_ptr<Layout> upLayout);
            virtual void execute();

        protected:

            std::unique_ptr<Layout> mupLayout;
        };

        // Job to remove layout
        class RemoveLayoutJob : public GUIJob
        {
        public:

            RemoveLayoutJob(GUI* pGUI, Layout const * pLayout);
            virtual void execute();

        protected:

            Layout const * mpLayout;
        };

        // Job to change value of config attribute
        class SetValueOfConfigAttributeJob : public GUIJob
        {
        public:

            SetValueOfConfigAttributeJob(GUI* pGUI, std::string attribute, std::string value);
            virtual void execute();

        protected:

            std::string mAttribute;
            std::string mValue;
        };

        // #####################################################################

        // Find index of layout, returns -1 if fails
        int findLayout(Layout const * pLayout) const;

        // Move layout
        void moveLayout(int oldIndex, int newIndex);

        // Internal resizing
        void internalResizing();

        // Members
        std::vector<std::unique_ptr<Layout> > mLayouts;
        int mWidth, mHeight;
        int mNewWidth, mNewHeight;
        CharacterSet mCharacterSet;
        std::unique_ptr<AssetManager> mupAssetManager;
        float mAccPeriodicTime;
        Config mConfig;
        Font const * mpDefaultFont;
        bool mResizing;
        float mResizeWaitTime;
        RenderItem const * mpResizeBlend;
        std::unique_ptr<localizationMap> mupLocalizationMap;
        std::vector<std::unique_ptr<GUIJob> > mJobs;
        std::unique_ptr<GazeDrawer> mupGazeDrawer;
        bool mDrawGazeVisualization;
        float mVectorGraphicsDPI;
        float mFontTallSize;
        float mFontMediumSize;
        float mFontSmallSize;
    };
}

#endif // GUI_H_
