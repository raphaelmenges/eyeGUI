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
        mpQuad = mpAssetManager->fetchRenderItem(shaders::Type::PICTURE, meshes::Type::QUAD);
        mpImage = mpAssetManager->fetchTexture(filepath);
        mAlignment = alignment;
    }

    void Image::transformAndSize(int x, int y, int width, int height)
    {

    }

    void Image::draw() const
    {

    }
}
