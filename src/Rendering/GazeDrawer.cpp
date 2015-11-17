//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GazeDrawer.h"

// TODO: testing
#include <iostream>

namespace eyegui
{
	GazeDrawer::GazeDrawer()
	{
		// Nothing to do
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
		
		const int pixelDistance = 10; // TODO: to config (pixels or percent of something?)
		
		if (points.empty() || glm::distance(gazePoint.point, points.back().point) > 10)
		{
			// Add new gaze point to points
			points.push_back(gazePoint);
		}
		else if(!points.empty())
		{
			// Increase size of last point
			points.back().size += tpf;

			// TODO: clamping
		}

		// Decrease alpha of all gaze points but last one
		int pointsToRemoveIndex = -1;
		for (int i = 0; i < points.size() - 1; i++)
		{
			points[i].alpha -= tpf; // TODO: config...
			if (points[i].alpha <= 0)
			{
				// Override index with higher one
				pointsToRemoveIndex = i;
			}
		}
		
		// Remove unnecessary points from vector
		if (pointsToRemoveIndex >= 0)
		{
			points.erase(points.begin(), points.begin() + pointsToRemoveIndex + 1);
		}

		std::cout << points.size() << std::endl;
	}

	void GazeDrawer::draw() const
	{
		// Draw lines

		// Draw focus cycles

	}
}