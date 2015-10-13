//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GUI.h"

#include "Defines.h"
#include "OperationNotifier.h"

namespace eyegui
{
    GUI::GUI(int width, int height)
    {
        // Initialize members
        mWidth = width;
        mHeight = height;
        mAccPeriodicTime = -(ACCUMULATED_TIME_PERIOD / 2);
        mLayoutsLocked = false;
        mConfigToLoad = NO_CONFIG_TO_LOAD;
        mupAssetManager = std::unique_ptr<AssetManager>(new AssetManager(this));

        // Input initialization
        mInput.mouseCursorX = width / 2;
        mInput.mouseCursorY = height / 2;

        // Initialize OpenGL
        mGLSetup.init();

        // TODO: test the freetype library
        mpGlyphQuad = mupAssetManager->fetchRenderItem(shaders::Type::PICTURE, meshes::Type::QUAD);
        mpFont = mupAssetManager->fetchFont("arial.ttf");
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
        // Would be not ok if layout could call resize, but it cannot (could otherwise cancel locking)
        mLayoutsLocked = true;
        {
            // Save to members
            mWidth = width;
            mHeight = height;

            // Resize all layouts
            for (std::unique_ptr<Layout>& upLayout : mLayouts)
            {
                // Layout fetches size via const pointer to this
                upLayout->resize();
            }

            // Resize font atlases
            mupAssetManager->resizeFontAtlases();
        }
        mLayoutsLocked = false;
    }

    void GUI::render(float tpf)
    {
        mLayoutsLocked = true;
        {
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

            // Restore OpenGL state of application
            mGLSetup.restore();
        }
        mLayoutsLocked = false;

        // TODO: test
        mpGlyphQuad->bind();
        mpGlyphQuad->getShader()->fillValue("matrix", glm::mat4(1.0f));
        mpGlyphQuad->getShader()->fillValue("alpha", 1);
        mpGlyphQuad->getShader()->fillValue("activity", 1);
        glBindTexture(GL_TEXTURE_2D, mpFont->getMediumTextureHandle());
        mpGlyphQuad->draw();
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
