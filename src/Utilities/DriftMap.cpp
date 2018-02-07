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
	DriftMap::DriftMap(GUI const * pGUI)
	{
		mpGUI = pGUI;
	}

	void DriftMap::update(int gazeX, int gazeY)
	{
		// Store current gaze for future drift estimations
		mGazeX = gazeX;
		mGazeY = gazeY;
	}

	void DriftMap::notifyInteraction(float centerX, float centerY)
	{
		// Calculate drift of interaction
		float driftX = (float)(mGazeX) - centerX;
		float driftY = (float)(mGazeY) - centerY;

		// Check into which cell to enter
		float cellSizeX = (float)(mpGUI->getWindowWidth()) / (float)(DriftGrid::RES_X);
		float cellSizeY = (float)(mpGUI->getWindowHeight()) / (float)(DriftGrid::RES_Y);
		int cellX = (float)(mGazeX) / cellSizeX;
		int cellY = (float)(mGazeY) / cellSizeY;

		// Override cell drift
		mGrid.cells[cellX][cellY] = std::make_pair(driftX, driftY);
	}

	void DriftMap::reset()
	{
		// Reset grid (slow but working)
		for (int x = 0; x < DriftGrid::RES_X; x++)
		{
			for (int y = 0; y < DriftGrid::RES_Y; y++)
			{
				mGrid.cells[x][y] = { 0.f, 0.f };
			}
		}
	}

	DriftGrid DriftMap::getCurrentDriftMap() const
	{
		DriftGrid copy;
		for (int x = 0; x < DriftGrid::RES_X; x++)
		{
			for (int y = 0; y < DriftGrid::RES_Y; y++)
			{
				copy.cells[x][y] = mGrid.cells[x][y];
			}
		}
		return copy;
	}

	void DriftMap::apply(int& rX, int& rY) const
	{
		float x = (float)rX;
		float y = (float)rY;
		apply(x, y);
		rX = (int)x;
		rY = (int)y;
	}

	void DriftMap::apply(float& rX, float& rY) const
	{
		// Copy the input
		glm::vec2 input(rX, rY);

		// Struct to hold cell info
		struct CellInfo
		{
			CellInfo(int x, int y, float distance) : x(x), y(y), distance(distance) {}
			int x; // x index of cell
			int y; // y index of cell
			float distance; // distance from cell center to input

			// Comparator function for sorting
			bool operator < (const CellInfo& rhs) const
			{
				return (distance < rhs.distance);
			}
		};

		// Cell sizes
		float cellSizeX = (float)(mpGUI->getWindowWidth()) / (float)(DriftGrid::RES_X);
		float cellSizeY = (float)(mpGUI->getWindowHeight()) / (float)(DriftGrid::RES_Y);

		// Vector with cell infos
		std::vector<CellInfo> cellInfos;
		cellInfos.reserve(DriftGrid::RES_X * DriftGrid::RES_Y);
		
		// Go over cells and push back cell info
		for (int x = 0; x < DriftGrid::RES_X; x++)
		{
			for (int y = 0; y < DriftGrid::RES_Y; y++)
			{
				// Center of cell
				glm::vec2 center;
				center.x = (x * cellSizeX) + (0.5f * cellSizeX);
				center.y = (y * cellSizeY) + (0.5f * cellSizeY);

				// Distance to input coordinate
				cellInfos.push_back(CellInfo(x, y, glm::distance(input, center)));
			}
		}

		// Sort after distance
		std::sort(cellInfos.begin(), cellInfos.end());

		// Check for zero distance
		float driftX = 0.f;
		float driftY = 0.f;
		if (!cellInfos.empty() && cellInfos.at(0).distance == 0)
		{
			auto cellDrift = mGrid.cells[cellInfos.at(0).x][cellInfos.at(0).y];
			driftX = cellDrift.first;
			driftY = cellDrift.second;
		}
		else // not zero distance to any cell center, interpolate!
		{

			// Go over the closest cells and aggregate the drift
			int neighborCount = glm::min(9, (int)cellInfos.size());
			glm::vec2 a(0, 0);
			glm::vec2 b(0, 0);
			for (int i = 0; i < neighborCount; i++)
			{
				float weight = 1.f / (cellInfos.at(i).distance * cellInfos.at(i).distance * cellInfos.at(i).distance);
				auto cellDrift = mGrid.cells[cellInfos.at(i).x][cellInfos.at(i).y];
				a += weight * glm::vec2(cellDrift.first, cellDrift.second);
				b += weight;
			}
			
			driftX = a.x / b.x;
			driftY = a.y / b.y;
		}

		// Apply
		rX -= driftX;
		rY -= driftY;
	}
}