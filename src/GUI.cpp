//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GUI.h"

#include "Defines.h"
#include "OperationNotifier.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

namespace eyegui
{
	GUI::GUI(
		int width,
		int height,
		std::string fontFilepath,
		CharacterSet characterSet,
		std::string localizationFilepath)
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

		// Initialize OpenGL
		mGLSetup.init();

		// Initialize default font ("" handled by asset manager)
		mpDefaultFont = mupAssetManager->fetchFont(fontFilepath);

		// Load initial localization
		if (localizationFilepath != EMPTY_STRING)
		{
			mupLocalizationMap = std::move(localization_parser::parse(localizationFilepath));
		}
		else
		{
			throwWarning(
				OperationNotifier::Operation::RUNTIME,
				"No localization filepath set. Localization will not work");
		}

		// Input initialization
		mInput.mouseCursorX = width / 2;
		mInput.mouseCursorY = height / 2;

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

	void GUI::resize(int width, int height)
	{
		// Not necessary but saves one from resizing after minimizing
		if (width > 0 && height > 0)
		{
			if (mWidth != width || mHeight != height)
			{
				mResizing = true;
				mResizeWaitTime = RESIZE_WAIT_DURATION;

				// Save to members
				mNewWidth = width;
				mNewHeight = height;
			}
		}
	}

	void GUI::render(float tpf)
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

		// TODO: just testing with mouse input
		mInput.mouseUsed = false;

		// Update all layouts in reversed order
		for (int i = (int)mLayouts.size() - 1; i >= 0; i--)
		{
			mLayouts[i]->update(tpf, &mInput);
		}

		// Setup OpenGL (therefore is draw not const)
		mGLSetup.setup(0, 0, getWindowWidth(), getWindowHeight());

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

		// Restore OpenGL state of application
		mGLSetup.restore();
	}

	void GUI::setMouseCursor(int x, int y)
	{
		mInput.mouseCursorX = x;
		mInput.mouseCursorY = y;
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

	void GUI::prefetchImage(std::string filepath)
	{
		// Do it immediately
		mupAssetManager->fetchTexture(filepath);
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
		// Set new width and height
		mWidth = mNewWidth;
		mHeight = mNewHeight;

		// Resize font atlases first
		mupAssetManager->resizeFontAtlases();

		// Then, resize all layouts
		for (std::unique_ptr<Layout>& upLayout : mLayouts)
		{
			// Layout fetches size via const pointer to this
			upLayout->resize();
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
}
