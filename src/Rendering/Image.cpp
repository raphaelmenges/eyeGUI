//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Image.h"

#include "Rendering/AssetManager.h"

namespace eyegui
{
    Image::Image(
        AssetManager* pAssetManager,
        std::string filepath,
        PictureAlignment alignment)
    {
        // Initialize members
        mpQuad = pAssetManager->fetchRenderItem(shaders::Type::IMAGE, meshes::Type::QUAD);
        mpTexture = pAssetManager->fetchTexture(filepath);
        mAlignment = alignment;
        mX = 0;
        mY = 0;
        mWidth = 0;
        mHeight = 0;
    }

    void Image::evaluateSize(
        int availableWidth,
        int availableHeight,
        int& rWidth,
        int& rHeight) const
    {
        if (mAlignment == PictureAlignment::ORIGINAL)
        {
            float availableAspectRatio = ((float)availableWidth) / ((float)availableHeight);
            float aspectRatio = (float)(mpTexture->getWidth()) / (float)(mpTexture->getHeight());

            if (availableAspectRatio < aspectRatio)
            {
                // Horizontal space less than necessary
                rWidth = availableWidth;

                // Adjust vertical size
                rHeight = (int)((float)rWidth / aspectRatio);
            }
            else
            {
                // Vertical space less than necessary
                rHeight = availableHeight;

                // Adjust horizontal size
                rWidth = (int)((float)rHeight * aspectRatio);
            }
        }
        else
        {
            rWidth = availableWidth;
            rHeight = availableHeight;
        }
    }

    void Image::transformAndSize(int x, int y, int width, int height)
    {
        mX = x;
        mY = y;
        mWidth = width;
        mHeight = height;
    }

    void Image::draw(glm::vec4 color) const
    {
        // Bind render item before setting values and drawing
        mpQuad->bind();

    }
}
