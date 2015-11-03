//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract encapsulation of the OpenGL texture creation and usage.

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "eyeGUI.h"

#include "Defines.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <string>
#include <vector>

namespace eyegui
{
	class Texture
	{
	public:

		enum class Filtering
		{
			NEAREST, LINEAR
		};

		enum class Wrap
		{
			CLAMP, MIRROR, REPEAT
		};

		// Constructor for pixel based image
		Texture();

		// Destructor
		virtual ~Texture() = 0;

		// Bind texture to slot for rendering
		void bind(uint slot = 0) const;

		// Getter for width and height
		uint getWidth() const;
		uint getHeight() const;

		// Getter for aspect ratio
		float getAspectRatio() const;

		// Getter for channel count of texture
		uint getChannelCount() const;

	protected:

		// Create OpenGL texture (something like initialization)
		void createOpenGLTexture(const std::vector<uchar>& rData, Filtering filtering, Wrap wrap, uint width, uint height, uint channelCount);

	private:

		// Flip image
		std::vector<uchar> flipImage(const std::vector<uchar>& rImage) const;

		// Members
		GLuint mTexture;
		uint mWidth;
		uint mHeight;
		uint mChannelCount;
	};
}

#endif // TEXTURE_H_
