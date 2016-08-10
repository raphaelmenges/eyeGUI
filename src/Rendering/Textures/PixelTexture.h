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
        PixelTexture(
			std::string filepath,
			Filtering filtering,
			Wrap wrap,
			int suspectedChannels = 0);

        PixelTexture(
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pData,
			bool flipY,
			Filtering filtering,
			Wrap wrap);

        // Destructor
        virtual ~PixelTexture();

		// Wrapping of image loading library, also used independently from this class's objects.
		// Returns success
		static bool loadImageFile(
			std::string filepath,
			std::vector<unsigned char>& rData,
			int& rWidth,
			int& rHeight,
			int& rChannelCount,
			int suspectedChannels = 0);
    };
}

#endif // PIXEL_TEXTURE_H_
