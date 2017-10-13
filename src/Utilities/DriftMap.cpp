//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DriftMap.h"

#include <iostream>

namespace eyegui
{
	DriftMap::DriftMap()
	{
		// Nothing to do here
	}

	void DriftMap::update(int gazeX, int gazeY)
	{
		mGazeX = gazeX;
		mGazeY = gazeY;
	}

	void DriftMap::notifyInteraction(int centerX, int centerY)
	{
		int driftX = mGazeX - centerX;
		int driftY = mGazeY - centerY;

		std::cout << "######################### HERE IS DRIFT MAP UPDATE" << std::endl;
		std::cout << "X: " << centerX << " Y: " << centerY << std::endl;
		std::cout << "DriftX: " << driftX << " DriftY: " << driftY << std::endl;

		// Update drift
		mDriftX = 0.5f * mDriftX + 0.5f * (float)driftX;
		mDriftY = 0.5f * mDriftY + 0.5f * (float)driftY;
	}

	void DriftMap::reset()
	{
		std::cout << "######################### HERE IS DRIFT MAP RESET" << std::endl;
		mDriftX = 0;
		mDriftY = 0;
	}

	void DriftMap::correct(int& gazeX, int& gazeY)
	{
		// Subtract drift from gaze
		gazeX -= mDriftX;
		gazeY -= mDriftY;
	}
}