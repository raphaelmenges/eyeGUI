//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// GUI class owning the layers with layouts. Most access through interface is
// handled using special job objects, which are executed before rendering.
// This ensures that the vector of layers is not changed during rendering and
// notifications from elements can trigger GUI jobs.

#ifndef GUI_H_
#define GUI_H_

#include "include/eyeGUI.h"

#include "Layer.h"
#include "Parser/ConfigParser.h"
#include "Parser/LocalizationParser.h"
#include "Rendering/AssetManager.h"
#include "Rendering/GLSetup.h"
#include "Config.h"
#include "Rendering/GazeDrawer.h"
#include "Utilities/Dictionary.h"

#include <memory>
#include <vector>
#include <functional>

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
            float fontSmallSize,
            FontSize descriptionFontSize,
			bool resizeInvisibleLayouts);

        // Destructor
        virtual ~GUI();

        // Load layout from xml, returns NULL if fails
        Layout* addLayout(std::string filepath, int layer, bool visible);

        // Remove layout
        void removeLayout(Layout const * pLayout);

        // Resize whole gui
        void resize(int width, int height);

        // Updating
		void update();
        Input update(float tpf, const Input input);

        // Drawing
        void draw() const;

        // Move layout to front in its layer
        void moveLayoutToFront(Layout* pLayout);

        // Move layout to back in its layer
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

        // Add dictionary. Returns index as handle
        uint addDictionary(std::string filepath);

        // Set whether descriptions are shown
        void setShowDescriptions(bool showDescriptions);

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

        // Get size for font
        float getSizeOfFont(FontSize fontSize) const;

        // Get pointer to dictionary, may be NULL
        Dictionary const * getDictionary(uint dictionaryIndex) const;

        // Get whether buttons should show description
        bool getShowDescriptions() const;

        // Get font size for descriptions
        FontSize getDescriptionFontSize() const;

		// Set function to callback after real resize
		void setResizeCallback(std::function<void(int, int)> callbackFunction);

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

            AddLayoutJob(GUI* pGUI, std::unique_ptr<Layout> upLayout, int layer);
            virtual void execute();

        protected:

            std::unique_ptr<Layout> mupLayout;
            int mLayer;
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

        // Internal resizing
        void internalResizing();

        // Typedefs
        typedef std::pair<int, std::unique_ptr<Layer> > LayerPair;

        // Members
        std::vector<std::unique_ptr<LayerPair> > mLayers;
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
        std::unique_ptr<LocalizationMap> mupLocalizationMap;
        std::vector<std::unique_ptr<GUIJob> > mJobs;
        std::unique_ptr<GazeDrawer> mupGazeDrawer;
        bool mDrawGazeVisualization;
        float mVectorGraphicsDPI;
        float mFontTallSize;
        float mFontMediumSize;
        float mFontSmallSize;
        std::vector<std::unique_ptr<Dictionary> > mDictionaries;
        bool mShowDescriptions;
        FontSize mDescriptionFontSize;
		bool mResizeInvisibleLayouts;
		std::function<void(int, int)> mResizeCallbackFunction;
		bool mResizeCallbackSet;
    };
}

#endif // GUI_H_
