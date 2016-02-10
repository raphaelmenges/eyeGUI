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
#include "Rendering/AssetManager.h"

#include <string>

namespace eyegui
{
    class Image
    {
    public:

        // Constructor
        Image(
            AssetManager* pAssetManager,
            std::string filepath,
            PictureAlignment alignment);

        // Deconstructor
        virtual ~Image();

        // Transform and size
        void transformAndSize(int x, int y, int width, int height);

        // Draw
        void draw() const;

    private:

        RenderItem const * mpQuad;
        Texture const * mpImage;
        PictureAlignment mAlignment;
    };
}

#endif // IMAGE_H_
