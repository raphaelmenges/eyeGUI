//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Structure for configuration.

#ifndef CONFIG_H_
#define CONFIG_H_

#include "src/Utilities/Helper.h"
#include "src/Defines.h"
#include "src/Style/StyleProperty.h"
#include "externals/GLM/glm/glm.hpp"

#include <map>

namespace eyegui
{
	// Deprecated
    class Config
    {
	public:

		// Constructor
		Config()
		{
			// TODO: delete this class
		}

		// Get a value
		std::shared_ptr<const StyleProperty<float> > getValue(StylePropertyFloat type) const { return std::shared_ptr<StyleProperty<float> >(new StyleProperty<float>(std::shared_ptr<const StyleClass>(), 1.0f)); }
		std::shared_ptr<const StyleProperty<glm::vec4> > getValue(StylePropertyVec4 type) const { return std::shared_ptr<StyleProperty<glm::vec4> >(new StyleProperty<glm::vec4>(std::shared_ptr<const StyleClass>(), glm::vec4(1.0f))); }

		// Set a value
		void setValue(StylePropertyFloat type, std::string value) { mFloatMap.at(type)->set(stringToFloat(value)); };
		void setValue(StylePropertyVec4 type, std::string value) { mVec4Map.at(type)->set(stringHexRGBAToVec4RGBA(value)); };

        // Initialize with fallback values
        std::string filepath;

		// Maps with values
		std::map < StylePropertyFloat, std::shared_ptr<StyleProperty<float> > > mFloatMap;
		std::map < StylePropertyVec4, std::shared_ptr<StyleProperty<glm::vec4> > > mVec4Map;

        // Experimental (TODO: change to new system too)
        float futureKeyboardPressDuration = 0.5f;
        float futureKeyboardRetriggerDelay = 0.5f;
        float futureKeyboardThresholdDuration = 1.f;
		float futureKeyboardRepeatKeyThresholdMultiplier = 1.f;
		float futureKeyboardSpaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardBackspaceKeyThresholdMultiplier = 1.f;
		float futureKeyboardSuggestionLineThresholdMultiplier = 1.f;

	protected:

		std::string mName = STYLE_BASE_CLASS_NAME;
    };
}

#endif // CONFIG_H_
