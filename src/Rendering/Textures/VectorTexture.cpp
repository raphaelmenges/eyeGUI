//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "VectorTexture.h"

#include "Helper.h"
#include "OperationNotifier.h"
#include "PathBuilder.h"

// NanoSVG wants those defines
#define NANOSVG_IMPLEMENTATION
#define NANOSVGRAST_IMPLEMENTATION

#include "externals/NanoSVG/nanosvg.h"
#include "externals/NanoSVG/nanosvgrast.h"

namespace eyegui
{
    VectorTexture::VectorTexture(std::string filepath, Filtering filtering, Wrap wrap) : Texture()
    {
        // Check file format
        if (!checkFileNameExtension(filepath, "svg"))
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Graphics file not found or wrong format", filepath);
        }

        // Parse file
        NSVGimage* svg = nsvgParseFromFile(buildPath(filepath).c_str(), "px", SVG_DPI);

        // Check whether file found and parsed
        if (svg == NULL)
        {
            throwError(OperationNotifier::Operation::IMAGE_LOADING, "Graphics file not found or error while parsing", filepath);
        }

        // Rasterize it and create OpenGL texture
        rasterizeGraphics(svg, filtering, wrap);

        // Delete graphics
        nsvgDelete(svg);
    }

    VectorTexture::VectorTexture(std::string const * pGraphic, Filtering filtering, Wrap wrap)
    {
        // Parse graphics
        char* str = static_cast<char*>(malloc(sizeof(char) * pGraphic->size() + 1));
        strcpy(str, pGraphic->data());
        NSVGimage* svg = nsvgParse(str, "px", SVG_DPI);
        free(str);

        // Rasterize it and create OpenGL texture
        rasterizeGraphics(svg, filtering, wrap);

        // Delete graphics
        nsvgDelete(svg);
    }

    VectorTexture::~VectorTexture()
    {
        // Nothing to do
    }

    void VectorTexture::rasterizeGraphics(NSVGimage* svg, Filtering filtering, Wrap wrap)
    {
        // TODO: Size depending on image scale. Better depend on rendering resolution

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
        createOpenGLTexture(image, filtering, wrap, width, height, channelCount);

        // Delete NanoSVG stuff
        nsvgDeleteRasterizer(rast);
    }
}
