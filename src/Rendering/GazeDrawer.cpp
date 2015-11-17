//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GazeDrawer.h"

#include "GUI.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

// TODO: testing
#include <iostream>

namespace eyegui
{
    GazeDrawer::GazeDrawer(GUI const * pGUI, AssetManager* pAssetManager)
    {
        // Members
        mpGUI = pGUI;
        mpAssetManager = pAssetManager;
        mpCircle = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE, meshes::Type::QUAD);
    }

    GazeDrawer::~GazeDrawer()
    {
        // Nothing to do
    }

    void GazeDrawer::update(int gazeX, int gazeY, float tpf)
    {
        // Get values from GUI
        float GUIWidth = (float)(mpGUI->getWindowWidth());
        float GUIHeight = (float)(mpGUI->getWindowHeight());

        // Maximal and minimal size of points
        float minGazePointRelativeSize = 0.02f; // TODO: config
        float maxGazePointRelativeSize = 0.075f; // TODO: config

        if (GUIWidth > GUIHeight)
        {
            mMinGazePointSize = (int) (minGazePointRelativeSize * GUIHeight);
            mMaxGazePointSize = (int) (maxGazePointRelativeSize * GUIHeight);
        }
        else
        {
            mMinGazePointSize = (int) (minGazePointRelativeSize * GUIWidth);
            mMaxGazePointSize = (int) (maxGazePointRelativeSize * GUIWidth);
        }

        // Current point
        GazePoint gazePoint(gazeX, gazeY);

        // Decide how to react
        if (mPoints.empty())
        {
            // No points to far, so just add it
            mPoints.push_back(gazePoint);
        }
        else if(glm::distance(gazePoint.point, mPoints.back().originalPoint) > calculateDiameterOfGazePoint(mPoints.back()) / 2.0f)
        {
            // Add new gaze point to points when distance is bigger than old point's radius
            if(mPoints.back().focus.getValue() < 0.25f) // TODO: maybe something for the config?
            {
                // Last gaze point was not really a focus, forget it
                mPoints.back() = gazePoint;
            }
            else
            {
                // Add as new gaze point
                mPoints.push_back(gazePoint);
            }
        }
        else
        {
            // Increase focus of last point
            mPoints.back().focus.update(tpf); // TODO: speed in config

            // Move towards new position
            mPoints.back().point +=  tpf * (gazePoint.point - mPoints.back().point);
        }

        // Decrease alpha of all gaze points but last one
        int pointsToRemoveIndex = -1;
        for (int i = 0; i < mPoints.size() - 1; i++)
        {
            mPoints[i].alpha.update(-tpf); // TODO: config...

            // Remember which points to remove
            if (mPoints[i].alpha.getValue() <= 0)
            {
                // Override index with higher one
                pointsToRemoveIndex = i;
            }
        }

        // Remove unnecessary points from vector
        if (pointsToRemoveIndex >= 0)
        {
            mPoints.erase(mPoints.begin(), mPoints.begin() + pointsToRemoveIndex + 1);
        }

        std::cout << mPoints.size() << std::endl;
    }

    void GazeDrawer::draw() const
    {
        // Draw lines

        // Draw focus cycles
        mpCircle->bind();

        // Get values from GUI
        float GUIWidth = (float)(mpGUI->getWindowWidth());
        float GUIHeight = (float)(mpGUI->getWindowHeight());

        // Draw all circles
        for (const GazePoint& rGazePoint : mPoints)
        {
            // Width and height
            int diameter = calculateDiameterOfGazePoint(rGazePoint);
            float glWidth = diameter / GUIWidth;
            float glHeight = diameter / GUIHeight;

            // New matrix
            glm::mat4 matrix = glm::mat4(1.0f);

            // Translation
            matrix = glm::translate(
                matrix,
                glm::vec3((rGazePoint.point.x - diameter / 2) / GUIWidth,
                    (1.0f - ((rGazePoint.point.y - diameter / 2) / GUIHeight)) - glHeight,
                    0));

            // Scaling
            matrix = glm::scale(matrix, glm::vec3(glWidth, glHeight, 1));

            // Projection
            matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f) * matrix;

            // Set values
            mpCircle->getShader()->fillValue("matrix", matrix);
            mpCircle->getShader()->fillValue("color", glm::vec4(1,0,0,0.5f)); // TODO: config
            mpCircle->getShader()->fillValue("alpha", rGazePoint.alpha.getValue());

            // Draw
            mpCircle->draw();
        }
    }

    int GazeDrawer::calculateDiameterOfGazePoint(const GazePoint& rGazePoint) const
    {
         return (int) 2 * // Distance is used and mesh is from 0..1, so double the size
                (rGazePoint.focus.getValue() * mMaxGazePointSize
                + (1.0f - rGazePoint.focus.getValue()) * mMinGazePointSize);
    }
}
