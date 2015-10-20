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

        // Constructor
        LerpValue(float value, float min, float max)
        {
            mValue = value;
            mMin = min;
            mMax = max;
        }
        LerpValue(float value) : LerpValue(value, 0 , 1) {}
        LerpValue() : LerpValue(0, 0 , 1) {}

        // Destructor
        virtual ~LerpValue() {}

        // Update, returns updated raw value
        float update(float amount)
        {
            mValue += amount;
            mValue = clamp(mValue, mMin, mMax);
            return mValue;
        }

        // Update, returns updated raw value
        float update(float amount, bool subtract)
        {
            if(subtract)
            {
                return update(-amount);
            }
            else
            {
                return update(amount);
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
        }

    private:

        // If not private, gcc allows assigning simple float
        LerpValue& operator=(LerpValue other) {}

        // Members
        float mValue;
        float mMin;
        float mMax;
    };
}

#endif // LERP_VALUE_H_
