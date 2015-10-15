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
        CharacterSet characterSet)
    {
        // Initialize members
        mWidth = width;
        mHeight = height;
        mCharacterSet = characterSet;
        mAccPeriodicTime = -(ACCUMULATED_TIME_PERIOD / 2);
        mLayoutsLocked = false;
        mConfigToLoad = NO_CONFIG_TO_LOAD;
        mupAssetManager = std::unique_ptr<AssetManager>(new AssetManager(this));
        mpDefaultFont = NULL;
        mResizing = false;
        mResizeWaitTime = 0;

        // Initialize default font
        if(fontFilepath != "")
        {
            mpDefaultFont = mupAssetManager->fetchFont(fontFilepath);
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "No default font set, text rendering will not work");
        }

        // Input initialization
        mInput.mouseCursorX = width / 2;
        mInput.mouseCursorY = height / 2;

        // Initialize OpenGL
        mGLSetup.init();

        // Fetch render item for resize blending
        mpResizeBlend = mupAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::QUAD);

        // TODO: testing
        mupTextFlow = std::move(
            mupAssetManager->createTextFlow(
                FontSize::SMALL,
                TextFlowAlignment::LEFT,
                10,
                10,
                400,
                100,
                glm::vec4(0,0,1,1),
                u""));
    }

    GUI::~GUI()
    {
        // Nothing to do so far
    }

    Layout* GUI::addLayout(std::string filepath, bool visible)
    {
        if (!mLayoutsLocked)
        {
            // Parse layout
            std::unique_ptr<Layout> upLayout = mLayoutParser.parse(this, mupAssetManager.get(), filepath);

            // Move layout and return raw pointer
            Layout* pLayout = upLayout.get();
            mLayouts.push_back(std::move(upLayout));

            // Set visibility
            pLayout->setVisibility(visible, false);

            return pLayout;
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to add layout while layouts are blocked for example because of rendering");
            return NULL;
        }
    }

    void GUI::resize(int width, int height)
    {
        if(mWidth != width || mHeight != height)
        {
            mResizing = true;
            mResizeWaitTime = RESIZE_WAIT_DURATION;

            // Save to members
            mWidth = width;
            mHeight = height;
        }
    }

    void GUI::render(float tpf)
    {
        mLayoutsLocked = true;
        {
            // Resizing
            if(mResizing)
            {
                mResizeWaitTime -= tpf;

                // Resizing should take place?
                if(mResizeWaitTime <= 0)
                {
                    internalResizing();
                    mResizing = false;
                    mResizeWaitTime = 0;
                }
            }

            // Expensive but better then in the middle of rendering
            if (mConfigToLoad != NO_CONFIG_TO_LOAD)
            {
                mConfig = mConfigParser.parse(mConfigToLoad);
                mConfigToLoad = NO_CONFIG_TO_LOAD;
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

            // TODO: test
            mupTextFlow->draw(1);

            // Render resize blend
            if(mResizing)
            {
                glm::mat4 matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f);
                mpResizeBlend->bind();
                mpResizeBlend->getShader()->fillValue("matrix", matrix);
                mpResizeBlend->getShader()->fillValue("color", RESIZE_BLEND_COLOR);
                mpResizeBlend->getShader()->fillValue("alpha", 1.0f - 0.5f * (mResizeWaitTime / RESIZE_WAIT_DURATION));
                mpResizeBlend->draw();
            }

            // Restore OpenGL state of application
            mGLSetup.restore();
        }
        mLayoutsLocked = false;
    }

    void GUI::setMouseCursor(int x, int y)
    {
        mInput.mouseCursorX = x;
        mInput.mouseCursorY = y;
    }

    void GUI::moveLayoutToFront(Layout* pLayout)
    {
        if (!mLayoutsLocked)
        {
            int index = findLayout(pLayout);

            // Continue only if found
            if (index >= 0)
            {
                moveLayout(index, (int)(mLayouts.size())-1);
            }
            else
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "Tried to move layout which is not in GUI");
            }
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to move layout while layouts are blocked for example because of rendering");
        }
    }

    void GUI::moveLayoutToBack(Layout* pLayout)
    {
        if (!mLayoutsLocked)
        {
            int index = findLayout(pLayout);

            // Continue only if found
            if (index >= 0)
            {
                moveLayout(index, 0);
            }
            else
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "Tried to move layout which is not in GUI");
            }
        }
        else
        {
            throwWarning(
                OperationNotifier::Operation::RUNTIME,
                "Tried to move layout while layouts are blocked for example because of rendering");
        }
    }

    int GUI::getWindowWidth() const
    {
        return mWidth;
    }

    int GUI::getWindowHeight() const
    {
        return mHeight;
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

    void GUI::internalResizing()
    {
        // Would be not ok if layout could call resize, but it cannot because it has only cont pointer
        // (could otherwise cancel locking by accident)
        mLayoutsLocked = true;
        {
            // Resize font atlases first
            mupAssetManager->resizeFontAtlases();

            // Then, resize all layouts
            for (std::unique_ptr<Layout>& upLayout : mLayouts)
            {
                // Layout fetches size via const pointer to this
                upLayout->resize();
            }

            // TODO: test
            mupTextFlow->resize();
        }
        mLayoutsLocked = false;
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

    void GUI::loadConfig(std::string filepath)
    {
        // Do it later, not during updating or so (because could be called during that)
        mConfigToLoad = filepath;
    }

    void GUI::prefetchImage(std::string filepath)
    {
        mupAssetManager->fetchTexture(filepath);
    }
}
