//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE 
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

namespace eyegui
{
	struct Config
	{
		// Initialize with fallback values
		float animationDuration = 0.2f;
		float sensorPenetrationIncreaseDuration = 3.0f;
		float sensorPenetrationDecreaseDuration = 1.5f;
		float buttonThresholdIncreaseDuration = 1.0f;
		float buttonThresholdDecreaseDuration = 2.0f;
		float buttonPressingDuration = 0.3f;
		float sensorInteractionPenetrationAmount = 0.5f;
		float dimmingIncreaseDuration = 1.5f;
		float dimmingDecreaseDuration = 0.25f;
		float maximalAdaptiveScaleIncrease = 1.0f; // TODO: add to wiki
		float adaptiveScaleIncreaseDuration = 1.0f; // TODO: add to wiki
		float adaptiveScaleDecreaseDuration = 1.0f; // TODO: add to wiki
	};
}

#endif // CONFIG_H_
