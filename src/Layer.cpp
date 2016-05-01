//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Layer.h"

namespace eyegui
{
    Layer::Layer()
    {
        // Nothing to do
    }

    Layer::~Layer()
    {
        // Nothing to do
    }

    void Layer::update(float tpf, Input* pInput)
    {
        // Update all layouts in reversed order
        for (int i = (int)mLayouts.size() - 1; i >= 0; i--)
        {
            // Update and use input
            mLayouts[i]->update(tpf, pInput);
        }
    }

    void Layer::draw() const
    {
        // Draw all layouts
        for (uint i = 0; i < mLayouts.size(); i++)
        {
            mLayouts[i]->draw();
        }
    }

    void Layer::makeResizeNecessary(bool force)
    {
        for (std::unique_ptr<Layout>& upLayout : mLayouts)
        {
            // Layout fetches size via const pointer to GUI
            upLayout->makeResizeNecessary(force);
        }
    }

    void Layer::addLayout(std::unique_ptr<Layout> upLayout)
    {
        mLayouts.push_back(std::move(upLayout));
    }

    bool Layer::removeLayout(Layout const * pLayout)
    {
        // Saving count of layouts
        int layoutCount = (int)mLayouts.size();

        // Removing layout from vector
        mLayouts.erase(
            std::remove_if(
                mLayouts.begin(),
                mLayouts.end(),
                [&](std::unique_ptr<Layout> const & p)
                { // This predicate checks, whether raw pointer is same as given
                    return (p.get() == pLayout);
                }),
            mLayouts.end());

        // Check, whether something has changed
        if ((int)(mLayouts.size()) == layoutCount)
        {
            return false;
        }
        else
        {
            return true;
        }

    }

    int Layer::findLayout(Layout const * pLayout) const
    {
        // Try to find index of layout in vector
        int index = -1;
        for (uint i = 0; i < mLayouts.size(); i++)
        {
            if (mLayouts[i].get() == pLayout)
            {
                index = i;
                break;
            }
        }

        return index;
    }

    void Layer::moveLayout(int oldIndex, int newIndex)
    {
        std::unique_ptr<Layout> upLayout = std::move(mLayouts[oldIndex]);
        mLayouts.erase(mLayouts.begin() + oldIndex);

        if (newIndex >= (int)(mLayouts.size()))
        {
            mLayouts.push_back(std::move(upLayout));
        }
        else
        {
            mLayouts.insert(mLayouts.begin() + newIndex, std::move(upLayout));
        }
    }

    int Layer::getLayoutCount() const
    {
        return (int) mLayouts.size();
    }
}
