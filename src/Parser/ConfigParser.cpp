//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "ConfigParser.h"

#include "Defines.h"
#include "Helper.h"
#include "OperationNotifier.h"

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

            // Read file
            std::ifstream in(filepath.c_str(), std::ios::in);

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

                    // Go through the cases
                    if (left == "animation-duration")
                    {
                        config.animationDuration = std::stof(right);
                    }
                    else if (left == "sensor-penetration-increase-duration")
                    {
                        config.sensorPenetrationIncreaseDuration = std::stof(right);
                    }
                    else if (left == "sensor-penetration-decrease-duration")
                    {
                        config.sensorPenetrationDecreaseDuration = std::stof(right);
                    }
                    else if (left == "button-threshold-increase-duration")
                    {
                        config.buttonThresholdIncreaseDuration = std::stof(right);
                    }
                    else if (left == "button-threshold-decrease-duration")
                    {
                        config.buttonThresholdDecreaseDuration = std::stof(right);
                    }
                    else if (left == "button-pressing-duration")
                    {
                        config.buttonPressingDuration = std::stof(right);
                    }
                    else if (left == "sensor-interaction-penetration-amount")
                    {
                        config.sensorInteractionPenetrationAmount = std::stof(right);
                    }
					else if (left == "dimming-increase-duration")
					{
						config.dimmingIncreaseDuration = std::stof(right);
					}
					else if (left == "dimming-decrease-duration")
					{
						config.dimmingDecreaseDuration = std::stof(right);
					}
                    else
                    {
                        throwError(OperationNotifier::Operation::PARSING, "Unknown value on left side of '=': " + left, filepath);
                    }
                }
            }

            return config;
        }
    }
}
