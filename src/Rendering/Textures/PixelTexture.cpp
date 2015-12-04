//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "PixelTexture.h"

#include "Helper.h"
#include "OperationNotifier.h"
#include "externals/picoPNG/picopng.h"
#include "PathBuilder.h"

#include <fstream>

namespace eyegui
{
    PixelTexture::PixelTexture(std::string filepath, Filtering filtering, Wrap wrap) : Texture()
    {
        // Read image from disk
        std::vector<uchar> image;
        uint width, height, channelCount;
        loadPixelImage(filepath, image, width, height, channelCount);

        // Create OpenGL texture
        createOpenGLTexture(image, filtering, wrap, width, height, channelCount);
    }

    PixelTexture::~PixelTexture()
    {
        // Nothing to do
    }

    void PixelTexture::loadPixelImage(
        std::string filepath,
        std::vector<uchar>& rImage,
        uint& rWidth,
        uint& rHeight,
        uint& rChannelCount) const
    {

        // Check file format
        if (!checkFileNameExtension(filepath, "png"))
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Image file not found or wrong format", filepath);
        }

        // Read file
        std::ifstream in(buildPath(filepath).c_str(), std::ios::in | std::ios::binary);

        // Check whether file was found
        if (!in)
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Image file not found", filepath);
        }

        // Get size
        in.seekg(0, std::ios::end);
        std::streamsize size = in.tellg();

        // Read it
        in.seekg(0, std::ios::beg);
        std::vector<char> buffer((uint)(size));
        in.read(&(buffer[0]), (uint)(size));

        // Close file
        in.close();

        // Decode image
        ulong longWidth, longHeight;
        decodePNG(rImage, longWidth, longHeight, reinterpret_cast<uchar*>(&(buffer[0])), (uint)(size), false);
        rWidth = (uint)(longWidth);
        rHeight = (uint)(longHeight);

        // Calculate number of channels
        rChannelCount = (uint)(rImage.size() / (rWidth * rHeight * sizeof(uchar)));
    }
}
