//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Visualizes gaze.

#ifndef GAZE_DRAWER_H_
#define GAZE_DRAWER_H_

#include "AssetManager.h"
#include "LerpValue.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

namespace eyegui
{
    // Forward declaration
    class GUI;

    class GazeDrawer
    {
    public:

        // Constructor
        GazeDrawer(GUI const * pGUI, AssetManager* pAssetManager); // TODO: asset manager (create line asset ?!)

        // Destructor
        virtual ~GazeDrawer();

        // Update
        void update(int gazeX, int gazeY, float tpf);

        // Draw
        void draw() const;

        // Reset
        void reset();

    private:

        // Structure for gaze points
        struct GazePoint
        {
            glm::vec2 point; // Easier to handle than ivec2
            glm::vec2 originalPoint;
            LerpValue alpha;
            LerpValue focus;

            // Constructor
            GazePoint(int gazeX, int gazeY)
            {
                point.x = (float)gazeX;
                point.y = (float)gazeY;
                originalPoint = point; // Save original position
                alpha.setValue(1);
                focus.setValue(0);

            }
        };

        // Calculate pixel diameter of gaze point
        int calculateDiameterOfGazePoint(const GazePoint& rGazePoint) const;

        // Members
        GUI const * mpGUI;
        AssetManager* mpAssetManager;
        std::vector<GazePoint> mPoints;
        RenderItem const * mpCircle;
        RenderItem const * mpLine;
        int mMinGazePointSize;
        int mMaxGazePointSize;
    };
}

#endif // GAZE_DRAWER
