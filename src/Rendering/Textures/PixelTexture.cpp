//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "PixelTexture.h"

#include "Helper.h"
#include "OperationNotifier.h"

#include "PathBuilder.h"

// stb_image wants those defines
#define STB_IMAGE_IMPLEMENTATION

#include "externals/stb/stb_image.h"

namespace eyegui
{
    PixelTexture::PixelTexture(std::string filepath, Filtering filtering, Wrap wrap, int suspectedChannels) : Texture()
    {
        // Setup stb_image
        stbi_set_flip_vertically_on_load(true);

        // Try to load image
        int width, height, channelCount;
        unsigned char *data = stbi_load(buildPath(filepath).c_str(), &width, &height, &channelCount, suspectedChannels);

        // Check whether file was found and parsed
        if (data == NULL)
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Image file not found or error at parsing", filepath);
        }

        // Create vector out of data
        std::vector<unsigned char> image(data, data + width * height * channelCount);

        // Create OpenGL texture
        createOpenGLTexture(image, filtering, wrap, width, height, channelCount, filepath);

        // Delete raw image data
        stbi_image_free(data);
    }

    PixelTexture::~PixelTexture()
    {
        // Nothing to do
    }
}
