//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GUI.h"

#include "src/Defines.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Rendering/ScissorStack.h"
#include "src/Parser/LayoutParser.h"
#include "src/Parser/StyleParser.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <algorithm>

namespace eyegui
{
    GUI::GUI(
        int width,
        int height,
        std::string fontFilepath,
		KeyboardLayout keyboardLayout,
        std::string localizationFilepath,
        float vectorGraphicsDPI,
        float fontTallSize,
        float fontMediumSize,
        float fontSmallSize,
        FontSize descriptionFontSize,
        bool resizeInvisibleLayouts,
		bool useDriftMap)
    {
        // Initialize OpenGL
        GLSetup::init();

        // Initialize members
        mWidth = width;
        mHeight = height;
        mNewWidth = mWidth;
        mNewHeight = mHeight;
        mKeyboardLayout = keyboardLayout;
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
        mDescriptionFontSize = descriptionFontSize;
        mResizeInvisibleLayouts = resizeInvisibleLayouts;
		mDescriptionVisibility = DescriptionVisibility::ON_PENETRATION;
        mResizeCallbackSet = false;
		mupDriftMap = std::unique_ptr<DriftMap>(new DriftMap(this));
		mUseDriftMap = useDriftMap;

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

		// Create style tree
		mspStyleTree = std::shared_ptr<StyleTree>(new StyleTree());
    }

    GUI::~GUI()
    {
        // Nothing to do so far
    }

    Layout* GUI::addLayout(std::string filepath, int layer, bool visible)
    {
        // Parse layout
		std::unique_ptr<Layout> upLayout = layout_parser::parse(this, mupAssetManager.get(), mupDriftMap.get(), filepath);

        // Get raw pointer to return
        Layout* pLayout = upLayout.get();

        // Set visibility
        pLayout->setVisibility(visible, false);

        // Do some initial resize to be ok for first draw
        pLayout->makeResizeNecessary(true, true);

        // Give unique pointer to job so it will be pushed back before next rendering but not during
        mJobs.push_back(std::move(std::unique_ptr<GUIJob>(new AddLayoutJob(this, std::move(upLayout), layer))));

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

    void GUI::update()
    {
        // Execute all jobs
        for (std::unique_ptr<GUIJob>& rupJob : mJobs)
        {
            rupJob->execute();
        }
        mJobs.clear();

        // Force resizing
        internalResizing();
        mResizing = false;
        mResizeWaitTime = 0;

        // Update all layers in reversed order
        for (int i = (int)mLayers.size() - 1; i >= 0; i--)
        {
            // Update without delta time or input
            mLayers[i]->second->update(0, NULL);
        }

        // Call resize callback (after update of layouts)
        if (mResizeCallbackSet)
        {
            mResizeCallbackFunction(mWidth, mHeight);
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
        bool resized = false;
        if (mResizing)
        {
            mResizeWaitTime -= tpf;

            // Resizing should take place?
            if (mResizeWaitTime <= 0)
            {
                internalResizing();
                mResizing = false;
                mResizeWaitTime = 0;
                resized = true;
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

		// Update drift map
		mupDriftMap->update(copyInput.gazeX, copyInput.gazeY);

		// Apply drift map
		if (mUseDriftMap)
		{
			mupDriftMap->apply(copyInput.gazeX, copyInput.gazeY);
		}

        // Update all layers in reversed order
        for (int i = (int)mLayers.size() - 1; i >= 0; i--)
        {
            // Update and use input
            mLayers[i]->second->update(tpf, &copyInput);
        }

        // Update gaze drawer
        mupGazeDrawer->update(input.gazeX, input.gazeY, tpf);

        // Call resize callback (after update of layouts)
        if (resized && mResizeCallbackSet)
        {
            mResizeCallbackFunction(mWidth, mHeight);
        }

        // Return copy of used input
        return copyInput;
    }

    void GUI::draw() const
    {
        // Setup OpenGL
        GLSetup glSetup;
        glSetup.setup(0, 0, getWindowWidth(), getWindowHeight());

        // Init scissor stack for this frame
        initScissorStack(getWindowWidth(), getWindowHeight());

        // Draw all layers
        for (uint i = 0; i < mLayers.size(); i++)
        {
            mLayers[i]->second->draw();
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

    void GUI::loadStyleSheet(std::string filepath)
    {
		// Attach content of file to style tree (TODO: could be executed while updating. not good). But if delayed to update, no layout / element can be added...
		style_parser::parse(mspStyleTree, filepath);
    }

    void GUI::setGazeVisualizationDrawing(bool draw)
    {
        mDrawGazeVisualization = draw;
    }

    void GUI::toggleGazeVisualizationDrawing()
    {
        mDrawGazeVisualization = !mDrawGazeVisualization;
    }

	void GUI::resetDriftMap()
	{
		mupDriftMap->reset();
	}

	DriftGrid GUI::getCurrentDriftMap() const
	{
		return mupDriftMap->getCurrentDriftMap();
	}

	void GUI::applyDriftMap(float &rX, float &rY) const
	{
		mupDriftMap->apply(rX, rY);
	}

	void GUI::prefetchImage(std::string filepath)
	{
		// Do it immediately
		mupAssetManager->fetchTexture(filepath);
	}

    uint GUI::addDictionary(std::string filepath)
    {
        // Do it immediately
        uint index = (uint)mDictionaries.size();
        std::unique_ptr<Dictionary> upDictionary = std::unique_ptr<Dictionary>(new Dictionary(filepath));
        mDictionaries.push_back(std::move(upDictionary));
        return index;
    }

    void GUI::setDescriptionVisibility(DescriptionVisibility visbility)
    {
		mDescriptionVisibility = visbility;
    }

	void GUI::setKeyboardLayout(KeyboardLayout keyboardLayout)
	{
		mJobs.push_back(std::move(std::unique_ptr<KeyboardLayoutJob>(new KeyboardLayoutJob(this, keyboardLayout))));
	}

	void GUI::playSound(std::string filepath)
	{
		mupAssetManager->playSound(filepath);
	}

	bool GUI::startAudioRecording()
	{
		return mupAssetManager->startAudioRecording();
	}

	bool GUI::endAudioRecording()
	{
		return mupAssetManager->endAudioRecording();
	}

	std::shared_ptr<const AudioRecord> GUI::retrieveAudioRecord() const
	{
		return mupAssetManager->retrieveAudioRecord();
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

	std::shared_ptr<const StyleTree> GUI::fetchStyleTree() const
    {
		return mspStyleTree;
    }

	KeyboardLayout GUI::getKeyboardLayout() const
    {
        return mKeyboardLayout;
    }

    Font const * GUI::getDefaultFont() const
    {
        return mpDefaultFont;
    }

    float GUI::getVectorGraphicsDPI() const
    {
        return mVectorGraphicsDPI;
    }

    float GUI::getSizeOfFont(FontSize fontSize) const
    {
        switch (fontSize)
        {
        case FontSize::TALL:
            return mFontTallSize;
            break;
        case FontSize::MEDIUM:
            return mFontMediumSize;
            break;
        case FontSize::SMALL:
            return mFontSmallSize;
            break;
        case FontSize::KEYBOARD:
            return FONT_KEYBOARD_SIZE;
            break;
        default:
            return 0;
            break;
        }
    }

    Dictionary const * GUI::getDictionary(uint dictionaryIndex) const
    {
        if((int)dictionaryIndex < (int)mDictionaries.size())
        {
            return mDictionaries[dictionaryIndex].get();
        }
        else
        {
            return NULL;
        }
    }

    DescriptionVisibility GUI::getDescriptionVisibility() const
    {
        return mDescriptionVisibility;
    }

    FontSize GUI::getDescriptionFontSize() const
    {
        return mDescriptionFontSize;
    }

    void GUI::setResizeCallback(std::function<void(int, int)> callbackFunction)
    {
        mResizeCallbackFunction = callbackFunction;
        mResizeCallbackSet = true;
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
			throwWarning(
				OperationNotifier::Operation::RUNTIME,
				"Tried to fetch localization but failed for key: " + key);
			return LOCALIZATION_NOT_FOUND;
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

		// Reset drift map
		mupDriftMap->reset();

        // Then, resize all layers
        for (auto& rLayer : mLayers)
        {
            rLayer->second->makeResizeNecessary(mResizeInvisibleLayouts);
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
        // Go over layers and try to remove layout
        int layerIndex = -1;
        int layoutIndex = -1;
        for (int i = 0; i < (int)mpGUI->mLayers.size(); i++)
        {
            layoutIndex = mpGUI->mLayers.at(i)->second->findLayout(mpLayout);
            if (layoutIndex >= 0)
            {
                layerIndex = i;
                break;
            }
        }

        // Continue only if found
        if (layoutIndex >= 0)
        {
            if (mToFront)
            {
                mpGUI->mLayers.at(layerIndex)->second->moveLayout(layoutIndex, (int)(mpGUI->mLayers.at(layerIndex)->second->getLayoutCount()) - 1);
            }
            else
            {
                mpGUI->mLayers.at(layerIndex)->second->moveLayout(layoutIndex, 0);
            }
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to move layout which is not in GUI");
        }
    }

    GUI::AddLayoutJob::AddLayoutJob(GUI* pGUI, std::unique_ptr<Layout> upLayout, int layer) : GUIJob(pGUI)
    {
        mupLayout = std::move(upLayout);
        mLayer = layer;
    }

    void GUI::AddLayoutJob::execute()
    {
        // Search for layer
        int layerIndex = -1;
        for(int i = 0; i < (int)mpGUI->mLayers.size(); i++)
        {
            if(mpGUI->mLayers[i]->first == mLayer)
            {
                layerIndex = i;
                break;
            }
        }

        // No layer found
        if(layerIndex < 0)
        {
            // Add layer first
            std::unique_ptr<LayerPair> pair = std::unique_ptr<LayerPair>(new LayerPair(mLayer, std::unique_ptr<Layer>(new Layer)));
            mpGUI->mLayers.push_back(std::move(pair));

            // Sort layers
            sort(
                mpGUI->mLayers.begin(),
                mpGUI->mLayers.end(),
                [](const std::unique_ptr<LayerPair>& a, const std::unique_ptr<LayerPair>& b)
                { // Comparator function
                    return a->first < b->first;
                });

            // Search again
            for(int i = 0; i < (int)mpGUI->mLayers.size(); i++)
            {
                if(mpGUI->mLayers[i]->first == mLayer)
                {
                    layerIndex = i;
                    break;
                }
            }
        }

        // Add to correspondig layer
        mpGUI->mLayers[layerIndex]->second->addLayout((std::move(mupLayout)));
    }

    GUI::RemoveLayoutJob::RemoveLayoutJob(GUI* pGUI, Layout const * pLayout) : GUIJob(pGUI)
    {
        mpLayout = pLayout;
    }

    void GUI::RemoveLayoutJob::execute()
    {
        // Bool for check
        bool check;

        // Go over layers and try to remove layout
        for (auto& rLayer: mpGUI->mLayers)
        {
            check = rLayer->second->removeLayout(mpLayout);
            if (check)
            {
                break;
            }
        }

        // Check, whether Layout was removed
        if(!check)
        {
            throwWarning(OperationNotifier::Operation::RUNTIME, "Tried to remove layout that did not exist");
        }

        // TODO: Remove layer if empty
    }

	GUI::KeyboardLayoutJob::KeyboardLayoutJob(GUI* pGUI, KeyboardLayout keyboardLayout) : GUIJob(pGUI)
	{
		mKeyboardLayout = keyboardLayout;
	}

	void GUI::KeyboardLayoutJob::execute()
	{
		// Set global keyboard layout
		mpGUI->mKeyboardLayout = mKeyboardLayout;
		
		// Go over all layouts and update keyboards
		for (auto& rLayer : mpGUI->mLayers)
		{
			rLayer->second->executeOnLayouts([](Layout& rLayout)
			{
				// Execute keyboard update on all elements of layout by calling it on root and then internally recursively
				rLayout.executeOnRoots([](Element* pElement)
				{
					// Finally, do that update keyboard thing
					pElement->updateKeyboardLayout();
				});
			});
		}
	}

	/* TODO
    GUI::SetValueOfConfigAttributeJob::SetValueOfConfigAttributeJob(GUI* pGUI, std::string attribute, std::string value) : GUIJob(pGUI)
    {
        mAttribute = attribute;
        mValue = value;
    }

    void GUI::SetValueOfConfigAttributeJob::execute()
    {
        config_parser::fillValue(mpGUI->mConfig, mAttribute, mValue, mpGUI->mConfig.filepath);
    }
	*/
}
