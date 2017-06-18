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
#include "src/Utilities/OperationNotifier.h"
#include "externals/PortAudio/include/portaudio.h"
#include "externals/stb/stb_vorbis.c"
#include <iostream>
#include <system_error>

namespace eyegui
{
	struct Audio
	{
		std::shared_ptr<short> spBuffer;
		int channelCount;
		int sampleCount;
		int bufferIndex;
	};

	// Static instance of audio that is available over complete runtime
	static Audio AUDIO;

	static int audioStreamCallback(
			const void * inputBuffer, // buffer for audio input via microphone
			void * outputBuffer, // buffer for audio output via speakers
			unsigned long framesPerBuffer, // counts of frames (count of samples for all channels)
			const PaStreamCallbackTimeInfo *timeInfo, // not used
			PaStreamCallbackFlags flags, // not used
			void * data) // not used
	{
		// Return value
		PaStreamCallbackResult result = PaStreamCallbackResult::paComplete;

		// Cast output buffer
		short *out = (short*)outputBuffer;

		// Prevent unused variable warnings
		(void)inputBuffer; 
		(void)timeInfo;
		(void)flags;
		(void)data;

		// Fill output buffer for PortAudio
		bool samplesLeft = AUDIO.bufferIndex < AUDIO.sampleCount;
		if (samplesLeft)
		{
			for (uint i = 0; i < framesPerBuffer; i++) // go over requested frames
			{
				for (uint j = 0; j < AUDIO.channelCount; j++) // go over channels
				{
					*out++ = AUDIO.spBuffer.get()[AUDIO.bufferIndex]; // add sample to output
					if (AUDIO.bufferIndex + 1 < AUDIO.sampleCount) // check whether there are samples left in the buffer
					{
						AUDIO.bufferIndex++;
					}
					else
					{
						samplesLeft = false;
						break; // break inner
					}
				}

				// break outer
				if (!samplesLeft)
				{
					break;
				}
			}
		}

		// Decide whether to continue
		if(samplesLeft)
		{
			result = PaStreamCallbackResult::paContinue;
		}
		return result;
	}

	static void audioStreamStopCallback(void * data)
	{
		// Prevent unused variable warnings
		(void)data;

		// TODO
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

		// Try to read audio file from disk
		std::string filepath = PathBuilder::buildFullFilepath("sounds/test.ogg");
		int sampleCount, channelCount, sampleRate = 0;
		short* buffer = NULL;
		sampleCount = stb_vorbis_decode_filename(filepath.c_str(), &channelCount, &sampleRate, &buffer);
		if(sampleCount < 0)
		{
			// TODO Failure
		}

		// Fill static audio structure
		AUDIO.spBuffer = std::shared_ptr<short>(buffer, free);
		AUDIO.channelCount = channelCount;
		AUDIO.sampleCount = sampleCount;
		AUDIO.bufferIndex = 0; // start with streaming at the beginning

		// PortAudio variables
		bool portAudioInitialized = false;
		PaStreamParameters parameters;
		PaStream* stream;
		PaError err;

		// Initialization
		err = Pa_Initialize();
		if (err != paNoError)
		{
			Pa_Terminate();
			OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
		}

		// Finding default device
		parameters.device = Pa_GetDefaultOutputDevice();
		parameters.channelCount = channelCount;
		parameters.sampleFormat = paInt16;
		parameters.suggestedLatency = Pa_GetDeviceInfo(parameters.device)->defaultLowOutputLatency;
		parameters.hostApiSpecificStreamInfo = NULL;
		if (parameters.device == paNoDevice)
		{
			Pa_Terminate();
			OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: Have not found an audio device");
		}
		else
		{
			portAudioInitialized = true;
		}

		// Start and execute a stream
		if (portAudioInitialized)
		{
			bool streamRunning = false;

			// Open stream
			err = Pa_OpenStream(
				&stream,
				NULL,
				&parameters,
				sampleRate,
				paFramesPerBufferUnspecified,
				paClipOff,
				audioStreamCallback,
				NULL);
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
			}

			// Set finish callback
			err = Pa_SetStreamFinishedCallback(stream, audioStreamStopCallback); // called when stream complete or stopped
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
			}

			// Start stream
			err = Pa_StartStream(stream);
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
			}
			else
			{
				streamRunning = true;
			}

			// Sleeping
			printf("Play for 5 seconds.\n");
			float duration = (float)sampleCount / (float)(sampleRate * channelCount);
			Pa_Sleep((long)(500 * duration));

			// Stop stream
			if (streamRunning)
			{
				err = Pa_StopStream(stream);
				if (err != paNoError)
				{
					OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
				}
				streamRunning = false;
			}
			
			// Close stream
			err = Pa_CloseStream(stream);
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
			}
		}

		// Terminate PortAudio
		if (portAudioInitialized)
		{
			Pa_Terminate();
		}
	}
}

#endif // TESTING_AREA_H_