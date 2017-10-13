//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DriftMap.h"

#include "src/GUI.h"

#include <iostream>

namespace eyegui
{
	DriftMap::DriftMap(GUI const * pGUI)
	{
		mpGUI = pGUI;
	}

	void DriftMap::update(int gazeX, int gazeY)
	{
		mGazeX = gazeX;
		mGazeY = gazeY;
	}

	void DriftMap::notifyInteraction(int centerX, int centerY)
	{
		// Drift
		float driftX = (float)(mGazeX - centerX);
		float driftY = (float)(mGazeY - centerY);

		// Debug output TODO: remove
		std::cout << "######################### HERE IS DRIFT MAP UPDATE" << std::endl;
		std::cout << "X: " << centerX << " Y: " << centerY << std::endl;
		std::cout << "DriftX: " << driftX << " DriftY: " << driftY << std::endl;

		// Update global drift
		mGlobalDriftX = 0.5f * mGlobalDriftX + 0.5f * driftX;
		mGlobalDriftY = 0.5f * mGlobalDriftY + 0.5f * driftY;

		// Update grid
		int gridX, gridY;
		calculateGridPoint(mGazeX, mGazeY, gridX, gridY);
		mGrid[gridX][gridY] = { driftX, driftY }; // simpe override entries
	}

	void DriftMap::reset()
	{
		std::cout << "######################### HERE IS DRIFT MAP RESET" << std::endl;

		// Reset global drift
		mGlobalDriftX = 0;
		mGlobalDriftY = 0;
		
		// Reset grid (slow but working)
		for (int x = 0; x <= RESOLUTION_X; x++)
		{
			for (int y = 0; y <= RESOLUTION_Y; y++)
			{
				mGrid[x][y] = { 0.f, 0.f };
			}
		}
	}

	void DriftMap::correct(int& gazeX, int& gazeY)
	{
		// Global drift
		/*
		gazeX -= mGlobalDriftX;
		gazeY -= mGlobalDriftY;
		*/

		// Grid with nearest neighbor interpolation
		int gridX, gridY;
		calculateGridPoint(gazeX, gazeY, gridX, gridY);
		gazeX -= (int)mGrid[gridX][gridY].first;
		gazeY -= (int)mGrid[gridX][gridY].second;
	}

	void DriftMap::calculateGridPoint(int coordX, int coordY, int& rGridPointX, int& rGridPointY) const
	{
		// TODO: breaks for 1x1 big window...
		float relativeX = (float)coordX / (float)(mpGUI->getWindowWidth() - 1); // relative value
		relativeX *= RESOLUTION_X; // value in interval [0,RESOLUTION]
		rGridPointX = (int)(relativeX + 0.5f);
		rGridPointX = rGridPointX < 0 ? 0 : (rGridPointX > (RESOLUTION_X + 1) ? (RESOLUTION_X + 1) : rGridPointX);
		float relativeY = (float)coordY / (float)(mpGUI->getWindowHeight() - 1); // relative value
		relativeY *= RESOLUTION_Y; // value in interval [0,RESOLUTION]
		rGridPointY = (int)(relativeY + 0.5f);
		rGridPointX = rGridPointY < 0 ? 0 : (rGridPointY >(RESOLUTION_Y + 1) ? (RESOLUTION_Y + 1) : rGridPointY);
	}
}