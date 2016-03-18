//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Internal vector based images.

#ifndef GRAPHICS_H_
#define GRAPHICS_H_

#include <string>

namespace eyegui
{
    namespace graphics
    {
        static const std::string circleGraphics =
            "<svg height=\"512\" width=\"512\">\n"
            "   <circle cx=\"256\" cy=\"256\" r=\"250\" stroke-width=\"0\" fill=\"white\"/>\n"
            "</svg>\n";

        static const std::string boxGraphics =
            "<svg height=\"10\" width=\"10\">\n"
            "   <rect x=\"0\" y=\"0\" width=\"10\" height=\"10\" stroke-width=\"0\" fill=\"white\"/>\n"
            "</svg>\n";

        static const std::string notFoundGraphics =
            "<svg height=\"100\" width=\"100\">\n"
            "   <rect\n"
            "       ry=\"0.90183705\"\n"
            "       rx=\"0.00013725708\"\n"
            "       y=\"60.022968\"\n"
            "       x=\"-38.686909\"\n"
            "       height=\"21.375404\"\n"
            "       width=\"77.37381\"\n"
            "       stroke-width=\"0\"\n"
            "       fill=\"red\"\n"
            "       transform=\"matrix(0.70710678,-0.70710678,0.70710678,0.70710678,0,0)\" />\n"
            "   <rect\n"
            "       transform=\"matrix(0.70710678,0.70710678,-0.70710678,0.70710678,0,0)\"\n"
            "       stroke-width=\"0\"\n"
            "       fill=\"red\"\n"
            "       width=\"77.37381\"\n"
            "       height=\"21.375404\"\n"
            "       x=\"32.023766\"\n"
            "       y=\"-10.687698\"\n"
            "       rx=\"0.00013725708\"\n"
            "       ry=\"0.90183705\" />\n"
            "</svg>\n";
    }
}

#endif // GRAPHICS_H_
