//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

#include "externals/GLM/glm/glm.hpp"

namespace eyegui
{
    struct Config
    {
        // Initialize with fallback values
        std::string filepath;
        float animationDuration = 0.1f;
        float sensorPenetrationIncreaseDuration = 3.0f;
        float sensorPenetrationDecreaseDuration = 1.5f;
        float buttonThresholdIncreaseDuration = 1.0f;
        float buttonThresholdDecreaseDuration = 2.0f;
        float buttonPressingDuration = 0.3f;
        float sensorInteractionPenetrationAmount = 0.5f;
        float dimIncreaseDuration = 1.5f;
        float dimDecreaseDuration = 0.25f;
        float maximalAdaptiveScaleIncrease = 0.5f;
        float adaptiveScaleIncreaseDuration = 1.0f;
        float adaptiveScaleDecreaseDuration = 1.0f;
        glm::vec4 gazeVisualizationColor = glm::vec4(0,0,1,0.5f);
        float gazeVisualizationFadeDuration = 4.0f;
        float gazeVisualizationFocusDuration = 2.0f;
        float gazeVisualizationRejectThreshold = 0.125f;
        float gazeVisualizationMinSize = 0.02f;
        float gazeVisualizationMaxSize = 0.075f;
        float keyboardSpeedMultiplier = 1.0f;
        float flowSpeedMultiplier = 1.0f;
    };
}

#endif // CONFIG_H_
