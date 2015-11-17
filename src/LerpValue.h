//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Linear interpolated value.

#ifndef LERP_VALUE_H_
#define LERP_VALUE_H_

#include "Helper.h"

namespace eyegui
{
	class LerpValue
	{
	public:

		// Constructors
		LerpValue(float value, float min, float max)
		{
			mMin = min;
			mMax = max;
			setValue(value);
		}
		LerpValue(float value) : LerpValue(value, 0, 1) {}
		LerpValue() : LerpValue(0, 0, 1) {}

		// Destructor
		virtual ~LerpValue() {}

		// Update, returns updated raw value
		float update(float delta)
		{
			mValue += delta;
			mValue = clamp(mValue, mMin, mMax);
			return mValue;
		}

		// Update, returns updated raw value
		float update(float delta, bool subtract)
		{
			if (subtract)
			{
				return update(-delta);
			}
			else
			{
				return update(delta);
			}
		}

		// Getter for value
		float getValue() const
		{
			return mValue;
		}

		// Setter for value
		void setValue(float value)
		{
			mValue = value;
			mValue = clamp(mValue, mMin, mMax);
		}

		// Setter for min value
		void setMin(float min)
		{
			mMin = min;
			mValue = clamp(mValue, mMin, mMax);
		}

		// Setter for max value
		void setMax(float max)
		{
			mMax = max;
			mValue = clamp(mValue, mMin, mMax);
		}

		// Getter for min value
		float getMin() const
		{
			return mMin;
		}

		// Getter for max value
		float getMax() const
		{
			return mMax;
		}

		// Assignment
		LerpValue& operator=(LerpValue other)
		{
			this->mValue = other.getValue();
			this->mMax = other.getMax();
			this->mMin = other.getMin();
			return *this;
		}

	private:

		// Members
		float mValue;
		float mMin;
		float mMax;
	};
}

#endif // LERP_VALUE_H_
