//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ConfigParser.h"

#include "src/Defines.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/PathBuilder.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <iostream>

namespace eyegui
{
    namespace config_parser
    {
        Config parse(std::string filepath)
        {
            // Check file name
            if (!checkFileNameExtension(filepath, CONFIG_EXTENSION))
            {
                throwError(OperationNotifier::Operation::PARSING, "Extension of file not as expected", filepath);
            }

            // Create config files with fallback values
            Config config;
            config.filepath = filepath;

            // Read file
            std::ifstream in(buildPath(filepath).c_str());

            // Check whether file was found
            if (!in)
            {
                throwError(OperationNotifier::Operation::PARSING, "Config file not found", filepath);
            }

            // Convert input file to string
            std::stringstream strStream;
            strStream << in.rdbuf();
            std::string content = strStream.str();

            // Close file
            in.close();

            // Streamline line endings
            streamlineLineEnding(content);

            // Get rid of whitespaces
            std::string::iterator end_pos = std::remove(content.begin(), content.end(), ' ');
            content.erase(end_pos, content.end());

            // Get rid of tabs
            end_pos = std::remove(content.begin(), content.end(), '\t');
            content.erase(end_pos, content.end());

            // Add some new line at the end of content because while loop does not read last line
            content += "\n";

            // Go through the lines
            std::string delimiter = "\n";
            size_t pos = 0;
            std::string line;
            while ((pos = content.find(delimiter)) != std::string::npos)
            {
                line = content.substr(0, pos);
                content.erase(0, pos + delimiter.length());

                // Only go on, if there is something
                if (line != "")
                {
                    // Get left and right side
                    std::string delimiter = "=";
                    size_t pos = 0;

                    if ((pos = line.find(delimiter)) == std::string::npos)
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Following line could not be parsed: " + line, filepath);
                        continue;
                    }

                    std::string left = line.substr(0, pos);
                    line.erase(0, pos + delimiter.length());
                    std::string right = line;

                    fillValue(config, left, right, filepath);
                }
            }

            return config;
        }

        void fillValue(Config& rConfig, std::string attribute, std::string value, std::string filepath)
        {
            // Go through the cases
            if (attribute == "animation-duration")
            {
				rConfig.setValue(StyleValue_float::AnimationDuration, value);
            }
            else if (attribute == "sensor-penetration-increase-duration")
            {
				rConfig.setValue(StyleValue_float::SensorPenetrationIncreaseDuration, value);
            }
            else if (attribute == "sensor-penetration-decrease-duration")
            {
				rConfig.setValue(StyleValue_float::SensorPenetrationDecreaseDuration, value);
            }
            else if (attribute == "button-threshold-increase-duration")
            {
				rConfig.setValue(StyleValue_float::ButtonThresholdIncreaseDuration, value);
            }
            else if (attribute == "button-threshold-decrease-duration")
            {
				rConfig.setValue(StyleValue_float::ButtonThresholdDecreaseDuration, value);
            }
            else if (attribute == "button-pressing-duration")
            {
				rConfig.setValue(StyleValue_float::ButtonPressingDuration, value);
            }
            else if (attribute == "sensor-interaction-penetration-amount")
            {
				rConfig.setValue(StyleValue_float::SensorInteractionPenetrationAmount, value);
            }
            else if (attribute == "dim-increase-duration")
            {
				rConfig.setValue(StyleValue_float::DimIncreaseDuration, value);
            }
            else if (attribute == "dim-decrease-duration")
            {
				rConfig.setValue(StyleValue_float::DimDecreaseDuration, value);
            }
			else if (attribute == "flash-duration")
			{
				rConfig.setValue(StyleValue_float::FlashDuration, value);
			}
            else if (attribute == "maximal-adaptive-scale-increase")
            {
				rConfig.setValue(StyleValue_float::MaximalAdaptiveScaleIncrease, value);
            }
            else if (attribute == "adaptive-scale-increase-duration")
            {
				rConfig.setValue(StyleValue_float::AdaptiveScaleIncreaseDuration, value);
            }
            else if (attribute == "adaptive-scale-decrease-duration")
            {
				rConfig.setValue(StyleValue_float::AdaptiveScaleDecreaseDuration, value);
            }
            else if (attribute == "gaze-visualization-color")
            {
				rConfig.setValue(StyleValue_vec4::GazeVisualizationColor, value);
            }
            else if (attribute == "gaze-visualization-fade-duration")
            {
				rConfig.setValue(StyleValue_float::GazeVisualizationFadeDuration, value);
            }
            else if (attribute == "gaze-visualization-focus-duration")
            {
				rConfig.setValue(StyleValue_float::GazeVisualizationFocusDuration, value);
            }
            else if (attribute == "gaze-visualization-reject-threshold")
            {
				rConfig.setValue(StyleValue_float::GazeVisualizationRejectThreshold, value);
            }
            else if (attribute == "gaze-visualization-min-size")
            {
				rConfig.setValue(StyleValue_float::GazeVisualizationMinSize, value);
            }
            else if (attribute == "gaze-visualization-max-size")
            {
				rConfig.setValue(StyleValue_float::GazeVisualizationMaxSize, value);
            }
            else if (attribute == "keyboard-zoom-speed-multiplier")
            {
				rConfig.setValue(StyleValue_float::KeyboardZoomSpeedMultiplier, value);
            }
			else if (attribute == "keyboard-key-press-duration")
			{
				rConfig.setValue(StyleValue_float::KeyboardKeyPressDuration, value);
			}
            else if (attribute == "flow-speed-multiplier")
            {
				rConfig.setValue(StyleValue_float::FlowSpeedMultiplier, value);
            }
			else if (attribute == "text-edit-scroll-speed-multiplier")
			{
				rConfig.setValue(StyleValue_float::TextEditScrollSpeedMultiplier, value);
			}
			/* TODO: Reintegrate config of future keyboard? Otherwise config is not loaded for it...
            else if (attribute == "future-keyboard-press-duration")
            {
                rConfig.futureKeyboardPressDuration = stringToFloat(value);
            }
            else if (attribute == "future-keyboard-retrigger-delay")
            {
                rConfig.futureKeyboardRetriggerDelay = stringToFloat(value);
            }
            else if (attribute == "future-keyboard-threshold-duration")
            {
                rConfig.futureKeyboardThresholdDuration = stringToFloat(value);
            }
			else if (attribute == "future-keyboard-repeat-key-threshold-multiplier")
			{
				rConfig.futureKeyboardRepeatKeyThresholdMultiplier = stringToFloat(value);
			}
			else if (attribute == "future-keyboard-space-key-threshold-multiplier")
			{
				rConfig.futureKeyboardSpaceKeyThresholdMultiplier = stringToFloat(value);
			}
			else if (attribute == "future-keyboard-backspace-key-threshold-multiplier")
			{
				rConfig.futureKeyboardBackspaceKeyThresholdMultiplier = stringToFloat(value);
			}
			else if (attribute == "future-keyboard-suggestion-line-threshold-multiplier")
			{
				rConfig.futureKeyboardSuggestionLineThresholdMultiplier = stringToFloat(value);
			}
			*/
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown value on left side of '=': " + attribute, filepath);
            }
        }
    }
}
