//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GUI.h"

#include "Defines.h"
#include "OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <algorithm>

namespace eyegui
{
    GUI::GUI(
        int width,
        int height,
        std::string fontFilepath,
        CharacterSet characterSet,
        std::string localizationFilepath,
        float vectorGraphicsDPI,
        float fontTallSize,
        float fontMediumSize,
        float fontSmallSize)
    {
        // Initialize members
        mWidth = width;
        mHeight = height;
        mNewWidth = 0;
        mNewHeight = 0;
        mCharacterSet = characterSet;
        mAccPeriodicTime = -(ACCUMULATED_TIME_PERIOD / 2);
        mupAssetManager = std::unique_ptr<AssetManager>(new AssetManager(this));
        mpDefaultFont = NULL;
        mResizing = false;
        mResizeWaitTime = 0;
        mupGazeDrawer = std::unique_ptr<GazeDrawer>(new GazeDrawer(this, mupAssetManager.get()));
        mDrawGazeVisualization = false;
        mVectorGraphicsDPI = vectorGraphicsDPI;
        mFontTallSize = fontTallSize;
        mFontMediumSize = fontMediumSize;
        mFontSmallSize = fontSmallSize;

        // Initialize OpenGL
        ogl_LoadFunctions();

        // Initialize default font ("" handled by asset manager)
        mpDefaultFont = mupAssetManager->fetchFont(fontFilepath);

        // Load initial localization
        if (localizationFilepath != EMPTY_STRING_ATTRIBUTE)
        {
            mupLocalizationMap = std::move(localization_parser::parse(localizationFilepath));
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "No localization filepath set. Localization will not work");
        }

        // Fetch render item for resize blending
        mpResizeBlend = mupAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);
    }

    GUI::~GUI()
    {
        // Nothing to do so far
    }

    Layout* GUI::addLayout(std::string filepath, bool visible)
    {
        // Parse layout
        std::unique_ptr<Layout> upLayout = layout_parser::parse(this, mupAssetManager.get(), filepath);

        // Get raw pointer to return
        Layout* pLayout = upLayout.get();

        // Set visibility
        pLayout->setVisibility(visible, false);

        // Give unique pointer to job so it will be pushed back before next rendering but not during
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new AddLayoutJob(this, std::move(upLayout)))));

        return pLayout;
    }

    void GUI::removeLayout(Layout const * pLayout)
    {
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new RemoveLayoutJob(this, pLayout))));
    }

    void GUI::resize(int width, int height)
    {
        // Not necessary but saves one from resizing after minimizing
        if (width > 0 && height > 0)
        {
            if (mWidth != width || mHeight != height)
            {
                // Initializes resizing, piped to layouts during updating
                mResizing = true;
                mResizeWaitTime = RESIZE_WAIT_DURATION;

                // Save to members
                mNewWidth = width;
                mNewHeight = height;
            }
        }
    }

    Input GUI::update(float tpf, const Input input)
    {
        // Execute all jobs
        for (std::unique_ptr<GUIJob>& rupJob : mJobs)
        {
            rupJob->execute();
        }
        mJobs.clear();

        // Resizing
        if (mResizing)
        {
            mResizeWaitTime -= tpf;

            // Resizing should take place?
            if (mResizeWaitTime <= 0)
            {
                internalResizing();
                mResizing = false;
                mResizeWaitTime = 0;
            }
        }

        // Handle time
        mAccPeriodicTime += tpf;
        if (mAccPeriodicTime > (ACCUMULATED_TIME_PERIOD / 2))
        {
            mAccPeriodicTime -= ACCUMULATED_TIME_PERIOD;
        }

        // Copy constant input
        Input copyInput = input;

        // Update all layouts in reversed order
        for (int i = (int)mLayouts.size() - 1; i >= 0; i--)
        {
            // Update and use input
            mLayouts[i]->update(tpf, &copyInput);
        }

        // Update gaze drawer
        mupGazeDrawer->update(input.gazeX, input.gazeY, tpf);

        // Return copy of used input
        return copyInput;
    }

    void GUI::draw() const
    {
        // Setup OpenGL
        GLSetup glSetup;
        glSetup.setup(0, 0, getWindowWidth(), getWindowHeight());

        // Draw all layouts
        for (int i = 0; i < mLayouts.size(); i++)
        {
            mLayouts[i]->draw();
        }

        // Render resize blend
        if (mResizing)
        {
            glm::mat4 matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
            mpResizeBlend->bind();
            mpResizeBlend->getShader()->fillValue("matrix", matrix);
            mpResizeBlend->getShader()->fillValue("color", RESIZE_BLEND_COLOR);
            // mpResizeBlend->getShader()->fillValue("alpha", 1.0f - 0.5f * (mResizeWaitTime / RESIZE_WAIT_DURATION));
            mpResizeBlend->getShader()->fillValue("alpha", 1.0f); // Without animation
            mpResizeBlend->draw();
        }

        // Draw gaze input
        if(mDrawGazeVisualization)
        {
            mupGazeDrawer->draw();
        }

        // Restore OpenGL state of application
        glSetup.restore();
    }

    void GUI::moveLayoutToFront(Layout* pLayout)
    {
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new MoveLayoutJob(this, pLayout, true))));
    }

    void GUI::moveLayoutToBack(Layout* pLayout)
    {
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new MoveLayoutJob(this, pLayout, false))));
    }

    void GUI::loadConfig(std::string filepath)
    {
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new LoadConfigJob(this, filepath))));
    }

    void GUI::setGazeVisualizationDrawing(bool draw)
    {
        mDrawGazeVisualization = draw;
    }

    void GUI::toggleGazeVisualizationDrawing()
    {
        mDrawGazeVisualization = !mDrawGazeVisualization;
    }

    void GUI::prefetchImage(std::string filepath)
    {
        // Do it immediately
        mupAssetManager->fetchTexture(filepath);
    }

    void GUI::setValueOfConfigAttribute(std::string attribute, std::string value)
    {
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new SetValueOfConfigAttributeJob(this, attribute, value))));
    }

    int GUI::getWindowWidth() const
    {
        return mWidth;
    }

    int GUI::getWindowHeight() const
    {
        return mHeight;
    }

    float GUI::getAccPeriodicTime() const
    {
        return mAccPeriodicTime;
    }

    Config const * GUI::getConfig() const
    {
        return &mConfig;
    }

    CharacterSet GUI::getCharacterSet() const
    {
        return mCharacterSet;
    }

    Font const * GUI::getDefaultFont() const
    {
        return mpDefaultFont;
    }

    std::u16string GUI::getContentFromLocalization(std::string key) const
    {
        auto it = mupLocalizationMap->find(key);

        if (it != mupLocalizationMap->end())
        {
            return it->second;
        }
        else
        {
            return LOCALIZATION_NOT_FOUND;
        }
    }

    float GUI::getVectorGraphicsDPI() const
    {
        return mVectorGraphicsDPI;
    }

    float GUI::getFontTallSize() const
    {
        return mFontTallSize;
    }

    float GUI::getFontMediumSize() const
    {
        return mFontMediumSize;
    }

    float GUI::getFontSmallSize() const
    {
        return mFontSmallSize;
    }

    int GUI::findLayout(Layout const * pLayout) const
    {
        // Try to find index of layout in vector
        int index = -1;
        for (int i = 0; i < mLayouts.size(); i++)
        {
            if (mLayouts[i].get() == pLayout)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    void GUI::moveLayout(int oldIndex, int newIndex)
    {
        std::unique_ptr<Layout> upLayout = std::move(mLayouts[oldIndex]);
        mLayouts.erase(mLayouts.begin() + oldIndex);

        if (newIndex >= mLayouts.size())
        {
            mLayouts.push_back(std::move(upLayout));
        }
        else
        {
            mLayouts.insert(mLayouts.begin() + newIndex, std::move(upLayout));
        }
    }

    void GUI::internalResizing()
    {
        // Actual resizing action
        mWidth = mNewWidth;
        mHeight = mNewHeight;

        // Resize font atlases first
        mupAssetManager->resizeFontAtlases();

        // Reset gaze drawer
        mupGazeDrawer->reset();

        // Then, resize all layouts
        for (std::unique_ptr<Layout>& upLayout : mLayouts)
        {
            // Layout fetches size via const pointer to this
            upLayout->makeResizeNecessary();
        }
    }

    GUI::GUIJob::GUIJob(GUI* pGUI)
    {
        mpGUI = pGUI;
    }

    GUI::MoveLayoutJob::MoveLayoutJob(GUI* pGUI, Layout* pLayout, bool toFront) : GUIJob(pGUI)
    {
        mpLayout = pLayout;
        mToFront = toFront;
    }

    void GUI::MoveLayoutJob::execute()
    {
        int index = mpGUI->findLayout(mpLayout);

        // Continue only if found
        if (index >= 0)
        {
            if (mToFront)
            {
                mpGUI->moveLayout(index, (int)(mpGUI->mLayouts.size()) - 1);
            }
            else
            {
                mpGUI->moveLayout(index, 0);
            }
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to move layout which is not in GUI");
        }
    }

    GUI::LoadConfigJob::LoadConfigJob(GUI *pGUI, std::string filepath) : GUIJob(pGUI)
    {
        mFilepath = filepath;
    }

    void GUI::LoadConfigJob::execute()
    {
        mpGUI->mConfig = config_parser::parse(mFilepath);
    }

    GUI::AddLayoutJob::AddLayoutJob(GUI* pGUI, std::unique_ptr<Layout> upLayout) : GUIJob(pGUI)
    {
        mupLayout = std::move(upLayout);
    }

    void GUI::AddLayoutJob::execute()
    {
        mpGUI->mLayouts.push_back(std::move(mupLayout));
    }

    GUI::RemoveLayoutJob::RemoveLayoutJob(GUI* pGUI, Layout const * pLayout) : GUIJob(pGUI)
    {
        mpLayout = pLayout;
    }

    void GUI::RemoveLayoutJob::execute()
    {
        // Saving count of layouts
        int layoutCount = (int) mpGUI->mLayouts.size();

        // Removing layout from vector
        mpGUI->mLayouts.erase(
            std::remove_if(
                mpGUI->mLayouts.begin(),
                mpGUI->mLayouts.end(),
                [&] (std::unique_ptr<Layout> const & p)
                { // This predicate checks, whether raw pointer is same as given
                    return (p.get() == mpLayout);
                }),
            mpGUI->mLayouts.end());

        // Check, whether something has changed
        if(mpGUI->mLayouts.size() == layoutCount)
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Tried to remove layout that did not exist");
        }
    }

    GUI::SetValueOfConfigAttributeJob::SetValueOfConfigAttributeJob(GUI* pGUI, std::string attribute, std::string value) : GUIJob(pGUI)
    {
        mAttribute = attribute;
        mValue = value;
    }

    void GUI::SetValueOfConfigAttributeJob::execute()
    {
        config_parser::fillValue(mpGUI->mConfig, mAttribute, mValue, mpGUI->mConfig.filepath);
    }
}
