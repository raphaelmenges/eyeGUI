//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Internal meshes used for rendering.

#ifndef MESHES_H_
#define MESHES_H_

#include <vector>

namespace eyegui
{
    namespace meshes
    {
        static const std::vector<float> quadVertices =
        {
            0.0,0.0,0, 1.0,0,0, 1.0,1.0,0,
            1.0,1.0,0, 0,1.0,0, 0,0,0
        };

        static const std::vector<float> quadTextureCoordinates =
        {
            0,0, 1,0, 1,1,
            1,1, 0,1, 0,0
        };

        static const std::vector<float> lineVertices =
        {
            0.0,0.0,0, 1.0,0,0
        };

        static const std::vector<float> lineTextureCoordinates =
        {
            0,0, 1,0
        };
    }
}

#endif // MESHES_H_
