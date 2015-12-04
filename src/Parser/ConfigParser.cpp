//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ConfigParser.h"

#include "Defines.h"
#include "Helper.h"
#include "OperationNotifier.h"
#include "ParserHelper.h"
#include "PathBuilder.h"

#include <algorithm>
#include <sstream>
#include <fstream>

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
            std::ifstream in(buildPath(filepath).c_str(), std::ios::in);

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
                rConfig.animationDuration = std::stof(value);
            }
            else if (attribute == "sensor-penetration-increase-duration")
            {
                rConfig.sensorPenetrationIncreaseDuration = std::stof(value);
            }
            else if (attribute == "sensor-penetration-decrease-duration")
            {
                rConfig.sensorPenetrationDecreaseDuration = std::stof(value);
            }
            else if (attribute == "button-threshold-increase-duration")
            {
                rConfig.buttonThresholdIncreaseDuration = std::stof(value);
            }
            else if (attribute == "button-threshold-decrease-duration")
            {
                rConfig.buttonThresholdDecreaseDuration = std::stof(value);
            }
            else if (attribute == "button-pressing-duration")
            {
                rConfig.buttonPressingDuration = std::stof(value);
            }
            else if (attribute == "sensor-interaction-penetration-amount")
            {
                rConfig.sensorInteractionPenetrationAmount = std::stof(value);
            }
            else if (attribute == "dimming-increase-duration")
            {
                rConfig.dimmingIncreaseDuration = std::stof(value);
            }
            else if (attribute == "dimming-decrease-duration")
            {
                rConfig.dimmingDecreaseDuration = std::stof(value);
            }
            else if (attribute == "maximal-adaptive-scale-increase")
            {
                rConfig.maximalAdaptiveScaleIncrease = std::stof(value);
            }
            else if (attribute == "adaptive-scale-increase-duration")
            {
                rConfig.adaptiveScaleIncreaseDuration = std::stof(value);
            }
            else if (attribute == "adaptive-scale-decrease-duration")
            {
                rConfig.adaptiveScaleDecreaseDuration = std::stof(value);
            }
            else if (attribute == "gaze-visualization-color")
            {
                rConfig.gazeVisualizationColor = parseColor(value);
            }
            else if (attribute == "gaze-visualization-fade-duration")
            {
                rConfig.gazeVisualizationFadeDuration = std::stof(value);
            }
            else if (attribute == "gaze-visualization-focus-duration")
            {
                rConfig.gazeVisualizationFocusDuration = std::stof(value);
            }
            else if (attribute == "gaze-visualization-reject-threshold")
            {
                rConfig.gazeVisualizationRejectThreshold = std::stof(value);
            }
            else if (attribute == "gaze-visualization-min-size")
            {
                rConfig.gazeVisualizationMinSize = std::stof(value);
            }
            else if (attribute == "gaze-visualization-max-size")
            {
                rConfig.gazeVisualizationMaxSize = std::stof(value);
            }
            else
            {
                throwError(OperationNotifier::Operation::PARSING, "Unknown value on left side of '=': " + attribute, filepath);
            }
        }
    }
}
