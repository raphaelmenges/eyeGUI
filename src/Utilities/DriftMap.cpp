//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DriftMap.h"

#include "src/GUI.h"
#include "src/Utilities/Helper.h"

#include "externals/GLM/glm/glm.hpp"

#include <iostream>
#include <cmath>

namespace eyegui
{
	float lerp(const float& oldValue, const float& newValue, const float& weight)
	{
		return oldValue * (1.f - weight) + newValue * weight;
	}

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
		
		auto n = calculateNearestGridVertices(rGazeX, rGazeY);	
		// 
		// if (different)
		// {
		// 	std::cout << "gridLowerX: " << grid.lowerX << " gridUpperX: " << grid.upperX << " gridLowerY: " << grid.lowerY << " gridUpperY: " << grid.upperY << " relativeX: " << grid.innerX << " relativeY: " << grid.innerY << std::endl;
		// }

		// Access values from vertices
		float driftAX = mGrid.verts[n.lowerX][n.lowerY].first * (1.f - n.innerX) + mGrid.verts[n.upperX][n.lowerY].first * n.innerX;
		float driftBX = mGrid.verts[n.lowerX][n.upperY].first * (1.f - n.innerX) + mGrid.verts[n.upperX][n.upperY].first * n.innerX;
		float driftX = driftAX * (1.f - n.innerY) + driftBX * n.innerY;
		float driftAY = mGrid.verts[n.lowerX][n.lowerY].second * (1.f - n.innerX) + mGrid.verts[n.upperX][n.lowerY].second * n.innerX;
		float driftBY = mGrid.verts[n.lowerX][n.upperY].second * (1.f - n.innerX) + mGrid.verts[n.upperX][n.upperY].second * n.innerX;
		float driftY = driftAY * (1.f - n.innerY) + driftBY * n.innerY;

		// Apply
		rGazeX -= (int)driftX;
		rGazeY -= (int)driftY;
	}

	void DriftMap::notifyInteraction(float centerX, float centerY)
	{
		// ###########################
		// ### UPDATE OF DRIFT MAP ###
		// ###########################

		// Calculate drift of interaction
		float driftX = (float)(mGazeX) - centerX;
		float driftY = (float)(mGazeY) - centerY;

		// Apply limitation to drift in pixels
		driftX = clamp(driftX, -DRIFT_MAP_MAX_DRIFT_PIXEL, DRIFT_MAP_MAX_DRIFT_PIXEL);
		driftY = clamp(driftY, -DRIFT_MAP_MAX_DRIFT_PIXEL, DRIFT_MAP_MAX_DRIFT_PIXEL);

		// // Debug output TODO: remove
		// std::cout << "######################### HERE IS DRIFT MAP UPDATE" << std::endl;
		// std::cout << "X: " << centerX << " Y: " << centerY << std::endl;
		// std::cout << "DriftX: " << driftX << " DriftY: " << driftY << std::endl;

		// Update global drift
		mGlobalDriftX = (0.5f * mGlobalDriftX) + (0.5f * driftX);
		mGlobalDriftY = (0.5f * mGlobalDriftY) + (0.5f * driftY);

		// Set nearest vertex of grid via nearest neighbor
		/*
		int gridX, gridY;
		calculateNearestGridVertex(mGazeX, mGazeY, gridX, gridY);
		mGrid.verts[gridX][gridY] = { driftX, driftY }; // simple override entries
		*/

		// Adapt values of nearest vertices by distance
		auto n = calculateNearestGridVertices(mGazeX, mGazeY); // input is gaze and its drift is corrected
		const float norm = 1.f / glm::sqrt(2.f);
		float oneMinusInnerX = 1.f - n.innerX;
		float oneMinusInnerY = 1.f - n.innerY;

		// Weights
		float ll = glm::length(glm::vec2(oneMinusInnerX, oneMinusInnerY)) * norm;
		float ul = glm::length(glm::vec2(n.innerX, oneMinusInnerY)) * norm;
		float uu = glm::length(glm::vec2(n.innerX, n.innerY)) * norm;
		float lu = glm::length(glm::vec2(oneMinusInnerX, n.innerY)) * norm;

		// Update of values
		mGrid.verts[n.lowerX][n.lowerY] = { lerp(mGrid.verts[n.lowerX][n.lowerY].first, driftX, ll), lerp(mGrid.verts[n.lowerX][n.lowerY].second, driftY, ll) };
		mGrid.verts[n.upperX][n.lowerY] = { lerp(mGrid.verts[n.upperX][n.lowerY].first, driftX, ul), lerp(mGrid.verts[n.upperX][n.lowerY].second, driftY, ul) };
		mGrid.verts[n.upperX][n.upperY] = { lerp(mGrid.verts[n.upperX][n.upperY].first, driftX, uu), lerp(mGrid.verts[n.upperX][n.upperY].second, driftY, uu) };
		mGrid.verts[n.lowerX][n.upperY] = { lerp(mGrid.verts[n.lowerX][n.upperY].first, driftX, lu), lerp(mGrid.verts[n.lowerX][n.upperY].second, driftY, lu) };
	}

	void DriftMap::reset()
	{
		// std::cout << "######################### HERE IS DRIFT MAP RESET" << std::endl;

		// Reset global drift
		mGlobalDriftX = 0;
		mGlobalDriftY = 0;
		
		// Reset grid (slow but working)
		for (int x = 0; x <= mGrid.RES_X; x++)
		{
			for (int y = 0; y <= mGrid.RES_Y; y++)
			{
				mGrid.verts[x][y] = { 0.f, 0.f };
			}
		}
	}

	DriftGrid DriftMap::getCurrentDriftMap() const
	{
		DriftGrid copy;
		for (int x = 0; x <= mGrid.RES_X; x++)
		{
			for (int y = 0; y <= mGrid.RES_Y; y++)
			{
				copy.verts[x][y] = mGrid.verts[x][y];
			}
		}
		return copy;
	}

	void DriftMap::calculateNearestGridVertex(int coordX, int coordY, int& rVertexX, int& rVertexY) const
	{
		float relativeX = (float)coordX / (float)(mpGUI->getWindowWidth() - 1); // relative value
		relativeX *= mGrid.RES_X; // value in interval [0,RES_X]
		rVertexX = (int)(relativeX + 0.5f);
		rVertexX = clamp(rVertexX, 0, mGrid.RES_X);
		float relativeY = (float)coordY / (float)(mpGUI->getWindowHeight() - 1); // relative value
		relativeY *= mGrid.RES_Y; // value in interval [0,RES_Y]
		rVertexY = (int)(relativeY + 0.5f);
		rVertexY = clamp(rVertexY, 0, mGrid.RES_Y);
	}

	DriftMap::GridPosition DriftMap::calculateNearestGridVertices(int coordX, int coordY) const
	{
		GridPosition result;
		float gridX = ((float)coordX / (float)(mpGUI->getWindowWidth() - 1)) * mGrid.RES_X;  // value in interval [0,RES_X]
		result.lowerX = (int)std::floor(gridX); result.lowerX = clamp(result.lowerX, 0, mGrid.RES_X);
		result.upperX = (int)std::ceil(gridX); result.upperX = clamp(result.upperX, 0, mGrid.RES_X);
		result.innerX = gridX - (float)result.lowerX; result.innerX = clamp(result.innerX, 0.f, 1.f); // relative position within grid cell
		float gridY = ((float)coordY / (float)(mpGUI->getWindowHeight() - 1)) * mGrid.RES_Y;  // value in interval [0,RES_Y]
		result.lowerY = (int)std::floor(gridY); result.lowerY = clamp(result.lowerY, 0, mGrid.RES_Y);
		result.upperY = (int)std::ceil(gridY); result.upperY = clamp(result.upperY, 0, mGrid.RES_Y);
		result.innerY = gridY - (float)result.lowerY; result.innerY = clamp(result.innerY, 0.f, 1.f); // relative position within grid cell
		return result;
	}
}