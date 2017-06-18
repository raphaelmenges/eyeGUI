//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Audio.h"

#include <stdlib.h> 

namespace eyegui
{
	Audio::Audio(int channelCount, int sampleCount, short* pBuffer) :
		mChannelCount(channelCount), mSampleCount(sampleCount), mpBuffer(pBuffer)
	{
		// Nothing to do
	}

	Audio::~Audio()
	{
		free(mpBuffer);
	}
}
