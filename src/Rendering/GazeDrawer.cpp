//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GazeDrawer.h"

#include "GUI.h"
#include "externals/GLM/glm/gtc/matrix_transform.hpp"

namespace eyegui
{
    GazeDrawer::GazeDrawer(GUI const * pGUI, AssetManager* pAssetManager)
    {
        // Members
        mpGUI = pGUI;
        mpAssetManager = pAssetManager;
        mpCircle = mpAssetManager->fetchRenderItem(shaders::Type::CIRCLE, meshes::Type::QUAD);
        mpLine = mpAssetManager->fetchRenderItem(shaders::Type::COLOR, meshes::Type::LINE);
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
        float minGazePointRelativeSize = mpGUI->getConfig()->gazeVisualizationMinSize;
        float maxGazePointRelativeSize = mpGUI->getConfig()->gazeVisualizationMaxSize;

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
            if(mPoints.back().focus.getValue() < mpGUI->getConfig()->gazeVisualizationRejectThreshold)
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
            mPoints.back().focus.update(tpf / mpGUI->getConfig()->gazeVisualizationFocusDuration);

            // Move towards new position along distance vector
            mPoints.back().point +=  tpf * (gazePoint.point - mPoints.back().point);
        }

        // Decrease alpha of all gaze points but last one
        int pointsToRemoveIndex = -1;
        for (uint i = 0; i < mPoints.size() - 1; i++)
        {
            mPoints[i].alpha.update(-tpf / mpGUI->getConfig()->gazeVisualizationFadeDuration);

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
    }

    void GazeDrawer::draw() const
    {
        // Get values from GUI
        float GUIWidth = (float)(mpGUI->getWindowWidth());
        float GUIHeight = (float)(mpGUI->getWindowHeight());

        // Draw lines
        mpLine->bind();

        // Go over connections of gaze points
        for(uint i = 0; i < mPoints.size() - 1; i++)
        {
            // Calculate transformation values
            glm::vec2 currentPoint = mPoints[i].point;
            glm::vec2 nextPoint = mPoints[i+1].point;
            float distance = glm::distance(currentPoint, nextPoint);

            // Matrix
            glm::mat4 matrix = glm::mat4(1.0f);

            // Translation
            matrix = glm::translate(
                matrix,
                glm::vec3(currentPoint.x / GUIWidth, 1.0f - (currentPoint.y / GUIHeight), 0));

            // Scaling
            matrix = glm::scale(matrix, glm::vec3(distance / GUIWidth, distance / GUIHeight, 1));

            // Rotation
            glm::vec2 vector = nextPoint-currentPoint;
            float angle = glm::acos(glm::dot(glm::vec2(1,0), vector) / glm::length(vector));

            // Decide direction of rotation
            if(nextPoint.y > currentPoint.y)
            {
                angle = -angle;
            }

            matrix = glm::rotate(matrix, angle, glm::vec3(0,0,1));

            // Projection
            matrix = glm::ortho(0.0f, 1.0f, 0.0f, 1.0f) * matrix;

            mpLine->getShader()->fillValue("matrix", matrix);
            mpLine->getShader()->fillValue("color", mpGUI->getConfig()->gazeVisualizationColor);
            mpLine->getShader()->fillValue("alpha", mPoints[i].alpha.getValue());
            mpLine->draw(GL_LINES);
        }

        // Draw focus cycles
        mpCircle->bind();

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
            mpCircle->getShader()->fillValue("color", mpGUI->getConfig()->gazeVisualizationColor);
            mpCircle->getShader()->fillValue("alpha", rGazePoint.alpha.getValue());

            // Draw
            mpCircle->draw();
        }
    }

    void GazeDrawer::reset()
    {
        mPoints.clear();
    }

    int GazeDrawer::calculateDiameterOfGazePoint(const GazePoint& rGazePoint) const
    {
         return (int) (2.0f * // Distance is used and mesh is from 0..1, so double the size
                (rGazePoint.focus.getValue() * mMaxGazePointSize
                + (1.0f - rGazePoint.focus.getValue()) * mMinGazePointSize));
    }
}
