//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Specialization of texture for pixel based images.

#ifndef PIXEL_TEXTURE_H_
#define PIXEL_TEXTURE_H_

#include "Texture.h"

namespace eyegui
{
    class PixelTexture : public Texture
    {
    public:

        // Constructor
        PixelTexture(std::string filepath, Filtering filtering, Wrap wrap, int suspectedChannels = 0);

        // Destructor
        virtual ~PixelTexture();
    };
}

#endif // PIXEL_TEXTURE_H_
