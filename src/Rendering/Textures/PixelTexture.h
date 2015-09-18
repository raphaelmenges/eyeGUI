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
		PixelTexture(std::string filepath, Filtering filtering, Wrap wrap);

		// Destructor
		virtual ~PixelTexture();

	protected:

		// Load pixel image from file
		void loadPixelImage(
			std::string filepath,
			std::vector<uchar>& rImage,
			uint& rWidth,
			uint& rHeight,
			uint& rChannelCount) const;
	};
}

#endif // PIXEL_TEXTURE_H_
