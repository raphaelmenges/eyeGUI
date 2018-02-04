//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Drift map. Is filled at interaction through dwell times activation of
// elements with the distance between center of the activated elements and the
// provided gaze coordinates. Offset is from center of activation to gaze.

// TODO: breaks for windows smaller than 1x1 (division through 0)
// TODO: this current design assumes that there is only one eyeGUI gui object
// but it would be better to have a shared drift map object which can be
// used and filled by multiple GUI objects. Or make the usage of multiple
// GUI objects no more necessary (e.g. in GazeTheWeb)

#ifndef DRIFT_MAP_H_
#define DRIFT_MAP_H_

#include "include/eyeGUI.h"

#include <algorithm>

namespace eyegui
{
	// Forward declaration
	class GUI;

	class DriftMap
	{
	public:

		// Constructor
		DriftMap(GUI const * pGUI);

		// Takes reference to gaze, stores it and provides correction
		void update(int& rGazeX, int& rGazeY);

		// Tell about center of attention and time until activation
		void notifyInteraction(float centerX, float centerY);

		// Reset drift map
		void reset();

		// Get copy of current drift map
		DriftGrid getCurrentDriftMap() const;

		// Apply
		void apply(float& rX, float& rY) const;

	private:

		// Calculate nearest grid vertices
		void calculateNearestGridVertex(int coordX, int coordY, int& rVertexX, int& rVertexY) const;

		// Calculate nearest grid vertices
		struct GridPosition
		{
			int lowerX, upperX, lowerY, upperY = 0; // grid vertices
			float innerX, innerY; // relative position within vertices
		};
		GridPosition calculateNearestGridVertices(int coordX, int coordY) const;

		// Pointer to GUI
		GUI const * mpGUI;

		// Current gaze
		int mGazeX = 0;
		int mGazeY = 0;

		// Global drift
		float mGlobalDriftX = 0;
		float mGlobalDriftY = 0;

		// Grid
		DriftGrid mGrid;
	};
}

#endif // DRIFT_MAP_H_