//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "PixelTexture.h"

#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"

// stb_image wants those defines
#define STB_IMAGE_IMPLEMENTATION

#include "externals/stb/stb_image.h"

namespace eyegui
{
    PixelTexture::PixelTexture(
		std::string filepath,
		Filtering filtering,
		Wrap wrap,
		int suspectedChannels) : Texture()
    {
        // Setup stb_image
        stbi_set_flip_vertically_on_load(true);

        // Try to load image
        int width, height, channelCount;
        unsigned char* data = stbi_load(buildPath(filepath).c_str(), &width, &height, &channelCount, suspectedChannels);

        // Check whether file was found and parsed
        if (data == NULL)
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Image file not found or error at parsing", filepath);
        }

		// Decide format
		GLenum glFormat;
		GLenum glInternalFormat;
		switch (suspectedChannels)
		{
		case 1:
			glFormat = GL_RED;
			glInternalFormat = GL_R8;
			break;
		case 3:
			glFormat = GL_RGB;
			glInternalFormat = GL_RGB8;
			break;
		case 4:
			glFormat = GL_RGBA;
			glInternalFormat = GL_RGBA8;
			break;
		default:
			glFormat= GL_RGB; 
			glInternalFormat = GL_R8;
			throwWarning(OperationNotifier::Operation::IMAGE_LOADING, "Unknown number of color channels", filepath);
			break;
		}

        // Create OpenGL texture
        createOpenGLTexture(data, filtering, wrap, width, height, channelCount, glFormat, glInternalFormat, false, filepath);

        // Delete raw image data
        stbi_image_free(data);
    }

    PixelTexture::PixelTexture(
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pData,
		bool flipY,
		Filtering filtering,
		Wrap wrap)
	{
		// Decide format
		uint channelCount = 4;
		GLenum glFormat;
		GLenum glInternalFormat;
		
		switch (format)
		{
		case ColorFormat::RGBA:
			glFormat = GL_RGBA;
			glInternalFormat = GL_RGBA;
			break;
		case ColorFormat::BGRA:
			glFormat = GL_BGRA;
			glInternalFormat = GL_RGBA;
			break;
		}

        // Create OpenGL texture
        createOpenGLTexture(pData, filtering, wrap, width, height, channelCount, glFormat, glInternalFormat, flipY, "No filepath");
    }

    PixelTexture::~PixelTexture()
    {
        // Nothing to do
    }
}
