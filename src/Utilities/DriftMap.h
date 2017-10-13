//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Drift map. Is filled at interaction through dwell times activation of
// elements with the offset between center of the activated elements and the
// provided gaze coordinates.

#ifndef DRIFT_MAP_H_
#define DRIFT_MAP_H_

namespace eyegui
{
	class DriftMap
	{
	public:

		// Constructor
		DriftMap();

		// Tell about center of attention and time until activation
		void updateDriftMap(int x, int y, float activationDuration);

		// Reset drift map
		void resetDriftMap();
	};
}

#endif // DRIFT_MAP_H_