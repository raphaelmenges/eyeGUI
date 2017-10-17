//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DriftMap.h"

#include "src/GUI.h"
#include "src/Utilities/Helper.h"

#include <iostream>
#include <cmath>

namespace eyegui
{
	DriftMap::DriftMap(GUI const * pGUI)
	{
		mpGUI = pGUI;
	}

	void DriftMap::update(int& rGazeX, int& rGazeY)
	{
		bool different = rGazeX != mGazeX && rGazeY != mGazeY;

		// #######################
		// ### UPDATE OF STATE ###
		// #######################

		// Store current gaze for future drift estimations
		mGazeX = rGazeX;
		mGazeY = rGazeY;

		// #######################################
		// ### APPLICATION OF DRIFT CORRECTION ###
		// #######################################

		// ### Global drift
		/*
		rGazeX -= mGlobalDriftX;
		rGazeY -= mGlobalDriftY;
		*/

		// ### Grid with nearest neighbor interpolation
		/*
		int gridX, gridY;
		calculateNearestGridVertex(rGazeX, rGazeY, gridX, gridY);
		rGazeX -= (int)mGrid[gridX][gridY].first;
		rGazeY -= (int)mGrid[gridX][gridY].second;
		*/

		// ### Grid with bilinear interpolation
		
		auto grid = calculateNearestGridVertices(rGazeX, rGazeY);	

		if (different)
		{
			std::cout << "gridLowerX: " << grid.lowerX << " gridUpperX: " << grid.upperX << " gridLowerY: " << grid.lowerY << " gridUpperY: " << grid.upperY << " relativeX: " << grid.innerX << " relativeY: " << grid.innerY << std::endl;
		}

		// Access values from vertices
		float driftAX = mGrid[grid.lowerX][grid.lowerY].first * (1.f - grid.innerX) + mGrid[grid.upperX][grid.lowerY].first * grid.innerX;
		float driftBX = mGrid[grid.lowerX][grid.upperY].first * (1.f - grid.innerX) + mGrid[grid.upperX][grid.upperY].first * grid.innerX;
		float driftX = driftAX * (1.f - grid.innerY) + driftBX * grid.innerY;
		float driftAY = mGrid[grid.lowerX][grid.lowerY].second * (1.f - grid.innerX) + mGrid[grid.upperX][grid.lowerY].second * grid.innerX;
		float driftBY = mGrid[grid.lowerX][grid.upperY].second * (1.f - grid.innerX) + mGrid[grid.upperX][grid.upperY].second * grid.innerX;
		float driftY = driftAY * (1.f - grid.innerY) + driftBY * grid.innerY;

		// Apply
		rGazeX -= (int)driftX;
		rGazeY -= (int)driftY;
	}

	void DriftMap::notifyInteraction(int centerX, int centerY)
	{
		// ###########################
		// ### UPDATE OF DRIFT MAP ###
		// ###########################

		// Calculate drift of interaction
		float driftX = (float)(mGazeX - centerX);
		float driftY = (float)(mGazeY - centerY);

		// Debug output TODO: remove
		std::cout << "######################### HERE IS DRIFT MAP UPDATE" << std::endl;
		std::cout << "X: " << centerX << " Y: " << centerY << std::endl;
		std::cout << "DriftX: " << driftX << " DriftY: " << driftY << std::endl;

		// Update global drift
		mGlobalDriftX = (0.5f * mGlobalDriftX) + (0.5f * driftX);
		mGlobalDriftY = (0.5f * mGlobalDriftY) + (0.5f * driftY);

		// Update nearest vertex of grid via nearest neighbor
		int gridX, gridY;
		calculateNearestGridVertex(mGazeX, mGazeY, gridX, gridY);
		mGrid[gridX][gridY] = { driftX, driftY }; // simple override entries
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

	void DriftMap::calculateNearestGridVertex(int coordX, int coordY, int& rVertexX, int& rVertexY) const
	{
		float relativeX = (float)coordX / (float)(mpGUI->getWindowWidth() - 1); // relative value
		relativeX *= RESOLUTION_X; // value in interval [0,RESOLUTION_X]
		rVertexX = (int)(relativeX + 0.5f);
		rVertexX = clamp(rVertexX, 0, RESOLUTION_X);
		float relativeY = (float)coordY / (float)(mpGUI->getWindowHeight() - 1); // relative value
		relativeY *= RESOLUTION_Y; // value in interval [0,RESOLUTION_X]
		rVertexY = (int)(relativeY + 0.5f);
		rVertexY = clamp(rVertexY, 0, RESOLUTION_Y);
	}

	DriftMap::GridPosition DriftMap::calculateNearestGridVertices(int coordX, int coordY) const
	{
		GridPosition result;
		float gridX = ((float)coordX / (float)(mpGUI->getWindowWidth() - 1)) * RESOLUTION_X;  // value in interval [0,RESOLUTION_X]
		result.lowerX = (int)std::floor(gridX); result.lowerX = clamp(result.lowerX, 0, RESOLUTION_X);
		result.upperX = (int)std::ceil(gridX); result.upperX = clamp(result.upperX, 0, RESOLUTION_X);
		result.innerX = gridX - (float)result.lowerX; result.innerX = clamp(result.innerX, 0.f, 1.f); // relative position within grid cell
		float gridY = ((float)coordY / (float)(mpGUI->getWindowHeight() - 1)) * RESOLUTION_Y;  // value in interval [0,RESOLUTION_Y]
		result.lowerY = (int)std::floor(gridY); result.lowerY = clamp(result.lowerY, 0, RESOLUTION_Y);
		result.upperY = (int)std::ceil(gridY); result.upperY = clamp(result.upperY, 0, RESOLUTION_Y);
		result.innerY = gridY - (float)result.lowerY; result.innerY = clamp(result.innerY, 0.f, 1.f); // relative position within grid cell
		return result;
	}
}