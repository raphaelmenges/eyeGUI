//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Encapsulation of the audio creation and usage.

#ifndef AUDIO_H_
#define AUDIO_H_

namespace eyegui
{
	class Audio
	{
	public:

		// Constructor. Handles freeing of buffer at destruction
		Audio(int channelCount, int sampleCount, short* pBuffer);

		// Destructor
		virtual ~Audio();

		// Getter
		int getChannelCount() const { return mChannelCount; }
		int getSampleCount() const { return mSampleCount; }
		short getSample(int index) const { return mpBuffer[index]; } // no bounds check performed!

	private:

		// Members
		int mChannelCount;
		int mSampleCount;
		short* mpBuffer;
	};
}

#endif // AUDIO_H_
