//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Testing area for new structures.


#ifndef TESTING_AREA_H_
#define TESTING_AREA_H_

#include "externals/GLM/glm/glm.hpp"
#include <memory>
#include <iostream>

namespace eyegui
{
	// General style value
	template<typename T>
	class StyleValue
	{
	public:

		// Constructor takes initial value
		StyleValue(T value) { mValue = value; }

		// Setter for value
		virtual void setValue(T value)
		{
			// Just store the value
			mValue = value;
		}

		// Getter for value
		T getValue() const { return mValue; }

	protected:

		// Protected default constructor called by subclass
		StyleValue() {}

		// Value
		T mValue;
	};

	// Numeric style value (which can be clamped)
	template<typename T>
	class NumericStyleValue : public StyleValue<T>
	{
	public:

		// Constructor takes initial value
		NumericStyleValue(T value) : StyleValue<T>(value) {}
		NumericStyleValue(T value, T min) { setMin(min); mValue = glm::max(value, min); }
		NumericStyleValue(T value, T min, T max) { setMin(min); setMax(max); mValue = glm::clamp(value, min, max); }

		// Setter for value. Includes clamping
		virtual void setValue(T value) override
		{
			if (mMinimumSet && mMaximumSet)
			{
				mValue = glm::clamp(value, mMinimum, mMaximum);
			}
			else if (mMinimumSet)
			{
				mValue = glm::max(value, mMinimum);
			}
			else
			{
				mValue = value;
			}
		}

	protected:

		// Set minimum
		void setMin(T min)
		{
			mMinimum = min;
			mMinimumSet = true;
		}

		// Set maximum
		void setMax(T max)
		{
			mMaximum = max;
			mMaximumSet = true;
		}

		// Minimum
		T mMinimum;
		bool mMinimumSet = false;

		// Maximum
		T mMaximum;
		bool mMaximumSet = false;
	};

	void testingMain()
	{
		// Declare raw values
		std::string intValue = "42";
		std::string hexValue = "0xFFFF00FF";
		std::string floatValue = "42.0";

		//auto spA = std::shared_ptr<StyleValue<float> >(new StyleValue<float>(42.f));
		//auto spB = std::shared_ptr<StyleValue<float> >(new ClampedStyleValue<float, 0.f, 1.f>(42.f));
		//auto spC = std::shared_ptr<StyleValue<float> >(new StyleValue<float>(42.f));

		//std::cout << spB->getValue() << std::endl;

		auto spA = std::shared_ptr<StyleValue<float> >(new StyleValue<float>(42.f));
		auto spB = std::shared_ptr<StyleValue<float> >(new NumericStyleValue<float>(42.f, 20.f));
		auto spC = std::shared_ptr<StyleValue<float> >(new NumericStyleValue<float>(42.f, 20.f, 30.f));

		std::cout << spC->getValue() << std::endl;
	}
}

#endif // TESTING_AREA_H_