//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Rendering of images used by picture element or all block elements as optional
// background.

#ifndef IMAGE_H_
#define IMAGE_H_

#include "include/eyeGUI.h"
#include "externals/GLM/glm/glm.hpp"

#include <string>

namespace eyegui
{
    // Forward declaration
    class Layout;
    class AssetManager;
    class RenderItem;
    class Texture;

    class Image
    {
    public:

        // Constructor
        Image(
            Layout const * pLayout,
            AssetManager* pAssetManager,
            std::string filepath,
            ImageAlignment alignment);

        // Deconstructor
        virtual ~Image();

        // Check before transformation, how much space is needed
        void evaluateSize(
            int availableWidth,
            int availableHeight,
            int& rWidth,
            int& rHeight) const;

        // Transform and size
        void transformAndSize(int x, int y, int width, int height);

        // Draw
        void draw(glm::vec4 color) const;

        // Getter for pixel sizes of used texture
        uint getTextureWidth() const;
        uint getTextureHeight() const;

    private:

        // Members
        Layout const * mpLayout;
        RenderItem const * mpQuad;
        Texture const * mpTexture;
        ImageAlignment mAlignment;
        int mX;
        int mY;
        int mWidth;
        int mHeight;
    };
}

#endif // IMAGE_H_
