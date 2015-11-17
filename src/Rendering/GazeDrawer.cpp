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
		GazePoint gazePoint;
		gazePoint.point.x = (float)gazeX;
		gazePoint.point.y = (float)gazeY;
		
		const int pixelDistance = 50; // TODO: to config (pixels or percent of something?)
		// TODO: not so good, because somehow frame rate dependent

		if (mPoints.empty() || glm::distance(gazePoint.point, mPoints.back().point) > pixelDistance)
		{
			// Add new gaze point to points
			mPoints.push_back(gazePoint);
		}
		else if(!mPoints.empty())
		{
			// Increase focus of last point
			mPoints.back().focus.update(tpf); // TODO: speed in config

			// Correct position
			mPoints.back().point = gazePoint.point;
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

		// Maximal and minimal size of points
		float maxGazePointSize = 0.2f; // TODO: config
		float minGazePointSize = 0.05f; // TODO: config
		int maxSize;
		int minSize;

		if (GUIWidth > GUIHeight)
		{
			maxSize = (int) (maxGazePointSize * GUIHeight);
			minSize = (int) (minGazePointSize * GUIHeight);
		}
		else
		{
			maxSize = (int) (maxGazePointSize * GUIWidth);
			minSize = (int) (minGazePointSize * GUIWidth);
		}

		// Draw all circles
		for (const GazePoint& rGazePoint : mPoints)
		{
			// Width and height
			int size = (int)
				(rGazePoint.focus.getValue() * maxSize
				+ (1.0f - rGazePoint.focus.getValue()) * minSize);
			float glWidth = size / GUIWidth;
			float glHeight = size / GUIHeight;

			// New matrix
			glm::mat4 matrix = glm::mat4(1.0f);

			// Translation
			matrix = glm::translate(
				matrix,
				glm::vec3((rGazePoint.point.x - size / 2) / GUIWidth,
					(1.0f - ((rGazePoint.point.y - size / 2) / GUIHeight)) - glHeight,
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
}