//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Drift map. Is filled at interaction through dwell times activation of
// elements with the distance between center of the activated elements and the
// provided gaze coordinates. Offset is from center of activation to gaze.

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
		void notifyActivation(int gazeX, int gazeY, int centerX, int centerY, float activationDuration);

		// Reset drift map
		void reset();

		// Correct drift
		void correct(int& gazeX, int& gazeY);

	private:

		// Global value of drift
		float mDriftX = 0;
		float mDriftY = 0;
	};
}

#endif // DRIFT_MAP_H_