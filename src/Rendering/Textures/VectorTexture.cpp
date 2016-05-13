//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "VectorTexture.h"

#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"

// NanoSVG wants those defines
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION

#include "externals/NanoSVG/nanosvg.h"
#include "externals/NanoSVG/nanosvgrast.h"

namespace eyegui
{
    VectorTexture::VectorTexture(std::string filepath, Filtering filtering, Wrap wrap, float dpi) : Texture()
    {
        // Parse file
        NSVGimage* svg = nsvgParseFromFile(buildPath(filepath).c_str(), "px", dpi);

        // Check whether file found and parsed
        if (svg == NULL)
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Graphics file not found or error while parsing", filepath);
        }

        // Rasterize it and create OpenGL texture
        rasterizeGraphics(svg, filtering, wrap, filepath);

        // Delete graphics
        nsvgDelete(svg);
    }

    VectorTexture::VectorTexture(std::string const * pGraphic, Filtering filtering, Wrap wrap, float dpi)
    {
        // Parse graphics
        char* str = static_cast<char*>(malloc(sizeof(char) * pGraphic->size() + 1));
        strcpy(str, pGraphic->data());
        NSVGimage* svg = nsvgParse(str, "px", dpi);
        free(str);

        // Rasterize it and create OpenGL texture
        rasterizeGraphics(svg, filtering, wrap, "");

        // Delete graphics
        nsvgDelete(svg);
    }

    VectorTexture::~VectorTexture()
    {
        // Nothing to do
    }

    void VectorTexture::rasterizeGraphics(NSVGimage* svg, Filtering filtering, Wrap wrap, std::string filepath)
    {
        uint width = (uint)(svg->width);
        uint height = (uint)(svg->height);
        const uint channelCount = 4;

        // Create rasterizer
        NSVGrasterizer* rast = nsvgCreateRasterizer();

        // Prepare memory for image
        std::vector<uchar> image;
        image.resize(width * height * channelCount);

        // Rasterize
        nsvgRasterize(rast, svg, 0, 0, 1, image.data(), width, height, width * channelCount);

        // Create OpenGL from image
		createOpenGLTexture(image.data(), filtering, wrap, width, height, channelCount, GL_RGBA, GL_RGBA, true, filepath);

        // Delete NanoSVG stuff
        nsvgDeleteRasterizer(rast);
    }
}
