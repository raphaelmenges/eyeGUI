//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Testing area for new structures.

#ifndef TESTING_AREA_H_
#define TESTING_AREA_H_

#include "src/Style/StyleTree.h"
#include "src/Utilities/PathBuilder.h"
#include "externals/PortAudio/include/portaudio.h"
#include "externals/stb/stb_vorbis.c"
#include <iostream>
#include <system_error>

namespace eyegui
{
	short* buffer;
	int bufferIndex = 0;
	int samples = 0;


	static int
		output_cb(const void * inputBuffer, void * outputBuffer, unsigned long frames_per_buffer,
			const PaStreamCallbackTimeInfo *time_info,
			PaStreamCallbackFlags flags, void * data)
	{
		short *out = (short*)outputBuffer;
		unsigned int i;
		(void)inputBuffer; /* Prevent unused variable warnings. */
		(void)time_info;
		(void)flags;
		(void)data;

		for (i = 0; i<frames_per_buffer; i++)
		{
			*out++ = buffer[bufferIndex];
			// TODO: channels
			if (bufferIndex + 1 < samples)
			{
				bufferIndex++;
			}
			else
			{
				break;
			}
			

			// TODO Do not overflow here. return paComplete instead
		}
		return paContinue;
	}

	static void
		end_cb(void * data)
	{
		printf("end!\n");
	}

	void testingMain()
	{
		/*
		// TODO
		StyleTree styleTree;
		auto header = styleTree.addStyleClass("header");
		auto headerButton = styleTree.addStyleClass("headerButton", "header");
		auto footer = styleTree.addStyleClass("footer");

		
		std::cout << header->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;

		styleTree.setValue("headerButton", StylePropertyFloat::AnimationDuration, "0.2");
		std::cout << header->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;

		styleTree.setValue("header", StylePropertyFloat::AnimationDuration, "0.2");
		std::cout << header->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;

		styleTree.setValue("header", StylePropertyFloat::AnimationDuration, "0");
		std::cout << header->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;
		std::cout << headerButton->fetchProperty(StylePropertyFloat::AnimationDuration)->get() << std::endl;
		*/
	
		// EXPERIMENT WITH STB VORBIS

		std::string filepath = PathBuilder::buildFullFilepath("sounds/test.ogg");
		int channels, sampleRate;
		samples = stb_vorbis_decode_filename(filepath.c_str(), &channels, &sampleRate, &buffer);
		std::cout << "Sample Rate: " << sampleRate << std::endl;
		std::cout << "Channel Number: " << channels << std::endl;
		std::cout << "Frame Number: " << samples << std::endl;

		PaStreamParameters out_param;
		PaStream * stream;

		// init portaudio
		Pa_Initialize();

		out_param.device = Pa_GetDefaultOutputDevice();
		if (out_param.device == paNoDevice)
		{
			fprintf(stderr, "Have not found an audio device!\n");
		}

		out_param.channelCount = channels;
		out_param.sampleFormat = paInt16;
		out_param.suggestedLatency = Pa_GetDeviceInfo(out_param.device)->defaultLowOutputLatency;
		out_param.hostApiSpecificStreamInfo = NULL;

		Pa_OpenStream(&stream, NULL, &out_param, sampleRate,
			paFramesPerBufferUnspecified, paClipOff,
			output_cb, NULL);

		Pa_SetStreamFinishedCallback(stream, &end_cb);

		Pa_StartStream(stream);

		printf("Play for 5 seconds.\n");
		float duration = (float)samples / (float)(sampleRate * channels);
		Pa_Sleep((long)(1000 * duration));

		Pa_StopStream(stream);

		Pa_CloseStream(stream);

		Pa_Terminate();

		delete buffer;
	}
}

#endif // TESTING_AREA_H_