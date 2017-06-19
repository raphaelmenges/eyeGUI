//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "AssetManager.h"

#include "Textures/PixelTexture.h"
#include "Textures/VectorTexture.h"
#include "Shaders.h"
#include "Meshes.h"
#include "Graphics.h"
#include "CharacterSets.h"
#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
#include "GUI.h"
#include "Font/AtlasFont.h"
#include "Font/EmptyFont.h"
#include "src/Utilities/PathBuilder.h"

#include "externals/stb/stb_vorbis.c" // should be included only one time in complete project

#include <algorithm>

// Static callback for PortAudio stream updates
static int audioStreamUpdateCallback(
	const void * inputBuffer, // buffer for audio input via microphone
	void * outputBuffer, // buffer for audio output via speakers
	unsigned long framesPerBuffer, // counts of frames (count of samples for all channels)
	const PaStreamCallbackTimeInfo *timeInfo, // not used
	PaStreamCallbackFlags flags, // not used
	void * data) // pointer to audio output data
{
	// Return value
	PaStreamCallbackResult result = PaStreamCallbackResult::paComplete;

	// Cast output buffer
	short *out = (short*)outputBuffer;

	// Prevent unused variable warnings
	(void)inputBuffer;
	(void)timeInfo;
	(void)flags;

	// Extract pointer to audio output data
	auto pData = reinterpret_cast<eyegui::AudioOutput*>(data);

	// Fill output buffer for PortAudio
	bool samplesLeft = pData->getIndex() < pData->getAudio()->getSampleCount();
	if (samplesLeft)
	{
		for (unsigned int i = 0; i < framesPerBuffer; i++) // go over requested frames
		{
			for (unsigned int j = 0; j < pData->getAudio()->getChannelCount(); j++) // go over channels
			{
				*out++ = pData->getAudio()->getSample(pData->getIndex()); // add sample to output
				if (pData->getIndex() + 1 < pData->getAudio()->getSampleCount()) // check whether there are samples left in the buffer
				{
					pData->incrementIndex();
				}
				else
				{
					samplesLeft = false;
					break; // break inner
				}
			}

			// Break outer
			if (!samplesLeft)
			{
				break;
			}
		}
	}

	// Decide whether to continue
	if (samplesLeft)
	{
		result = PaStreamCallbackResult::paContinue;
	}
	return result;
}

namespace eyegui
{
    AssetManager::AssetManager(GUI const * pGUI)
    {
        // Save members
        mpGUI = pGUI;

        // Initialize FreeType Library
        if (FT_Init_FreeType(&mFreeTypeLibrary))
        {
            throwError(
                OperationNotifier::Operation::RUNTIME,
                "Could not initialize FreeType Library");
        }

		// Initialization
		PaError err = Pa_Initialize();
		if (err != paNoError)
		{
			Pa_Terminate();
			OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
		}
		else
		{
			mPortAudioInitialized = true;
		}
    }

    AssetManager::~AssetManager()
    {
        // Get rid of fonts first because they have to delete FreeType faces
        mFonts.clear();

        // Close FreeType Library
        FT_Done_FreeType(mFreeTypeLibrary);

		// Terminate PortAudio
		if (mPortAudioInitialized)
		{
			Pa_Terminate();
		}
    }

    RenderItem const * AssetManager::fetchRenderItem(shaders::Type shader, meshes::Type mesh)
    {
        std::unique_ptr<RenderItem> upRenderItem;
        RenderItem* pRenderItem = NULL;

        // Fetch shader and mesh
        Shader const * pShader = fetchShader(shader);
        Mesh const * pMesh = fetchMesh(mesh);

        // Search in map for render item
        if (mRenderItems.find(shader) != mRenderItems.end())
        {
            if (mRenderItems[shader].find(mesh) != mRenderItems[shader].end())
            {
                // Found it!
                pRenderItem = mRenderItems[shader][mesh].get();
            }
            else
            {
                // Mesh not registered for this shader
                upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(pShader, pMesh));
                pRenderItem = upRenderItem.get();
                mRenderItems[shader][mesh] = std::move(upRenderItem);
            }
        }
        else
        {
            // Shader not registered
            upRenderItem = std::unique_ptr<RenderItem>(new RenderItem(pShader, pMesh));
            pRenderItem = upRenderItem.get();
            std::map<meshes::Type, std::unique_ptr<RenderItem> > innerMap;
            innerMap[mesh] = std::move(upRenderItem);
            mRenderItems.insert(make_pair(shader, std::move(innerMap)));
        }

        return pRenderItem;
    }

    Texture const * AssetManager::fetchTexture(std::string filepath)
    {
        // Search in map for texture and create if needed
        std::unique_ptr<Texture>& rupTexture = mTextures[filepath];
        Texture* pTexture = rupTexture.get();

        if (pTexture == NULL)
        {
            // Check for empty string
            if (filepath == "")
            {
                rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP, mpGUI->getVectorGraphicsDPI()));
            }
            else
            {
                // Determine image type
                std::string delimiter = ".";
                std::string input = filepath;
                size_t pos = 0;
                std::string token;
                while ((pos = input.find(delimiter)) != std::string::npos)
                {
                    token = input.substr(0, pos);
                    input.erase(0, pos + delimiter.length());
                }

                // File format case is not relevant
                std::transform(input.begin(), input.end(), input.begin(), ::tolower);

                // Check token
                if (input.compare("svg") == 0)
                {
                    rupTexture = std::unique_ptr<Texture>(new VectorTexture(filepath, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP, mpGUI->getVectorGraphicsDPI()));
                }
                else if (input.compare("png") == 0 || input.compare("jpg") == 0 || input.compare("jpeg") == 0 || input.compare("tga") == 0 || input.compare("bmp") == 0)
                {
                    // For PNG, suspect 4 channels (0 should do it for all cases, but stb_image always tells me about 3 channels and then moving to GPU fails)
                    int suspectedChannelCount = 3;
                    if(input.compare("png") == 0)
                    {
                        suspectedChannelCount = 4;
                    }

                    rupTexture = std::unique_ptr<Texture>(new PixelTexture(filepath, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP, suspectedChannelCount));
                }
                else
                {
                    throwWarning(OperationNotifier::Operation::IMAGE_LOADING, "Image file not found or wrong format. Replaced with placeholder", filepath);
                    rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP, mpGUI->getVectorGraphicsDPI()));
                }
            }
            pTexture = rupTexture.get();
            mTextures[filepath] = std::move(rupTexture);
        }

        return pTexture;
    }

    Texture const * AssetManager::fetchTexture(
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pData,
		bool flipY)
    {
        // Just overwrite it if existing
        mTextures[name] = std::move(std::unique_ptr<Texture>(new PixelTexture(width, height, format, pData, flipY, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP)));
        return mTextures[name].get();
    }

    Texture const * AssetManager::fetchTexture(graphics::Type graphic)
    {
        // Search in map for graphic and create if needed
        std::unique_ptr<Texture>& rupTexture = mGraphics[graphic];
        Texture* pTexture = rupTexture.get();

        if (pTexture == NULL)
        {
            switch (graphic)
            {
            case graphics::Type::CIRCLE:
                rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::circleGraphics, Texture::Filtering::LINEAR, Texture::Wrap::BORDER, mpGUI->getVectorGraphicsDPI()));
                break;
            case graphics::Type::BOX:
                rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::boxGraphics, Texture::Filtering::LINEAR, Texture::Wrap::CLAMP, mpGUI->getVectorGraphicsDPI()));
                break;
            case graphics::Type::NOT_FOUND:
                rupTexture = std::unique_ptr<Texture>(new VectorTexture(&graphics::notFoundGraphics, Texture::Filtering::LINEAR, Texture::Wrap::BORDER, mpGUI->getVectorGraphicsDPI()));
                break;
            default:
                throwError(OperationNotifier::Operation::BUG, "Graphics does not exist");
            }
            pTexture = rupTexture.get();
            mGraphics[graphic] = std::move(rupTexture);
        }

        return pTexture;
    }

    Shader const * AssetManager::fetchShader(shaders::Type shader)
    {
        // Search in map for shader and create if needed
        std::unique_ptr<Shader>& rupShader = mShaders[shader];
        Shader* pShader = rupShader.get();

        if (pShader == NULL)
        {
            switch (shader)
            {
            case shaders::Type::COLOR:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pColorFragmentShader));
                break;
            case shaders::Type::CIRCLE:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCircleFragmentShader));
                break;
            case shaders::Type::DIM:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pDimFragmentShader));
                break;
			case shaders::Type::FLASH:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pFlashFragmentShader));
				break;
            case shaders::Type::ACTIVITY:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pActivityFragmentShader));
                break;
            case shaders::Type::MARK:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pMarkFragmentShader));
                break;
            case shaders::Type::CIRCLE_THRESHOLD:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCircleThresholdFragmentShader));
                break;
            case shaders::Type::BOX_THRESHOLD:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pBoxThresholdFragmentShader));
                break;
            case shaders::Type::HIGHLIGHT:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pHighlightFragmentShader));
                break;
            case shaders::Type::SELECTION:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pSelectionFragmentShader));
                break;
            case shaders::Type::CIRCLE_BUTTON:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCircleButtonFragmentShader));
                break;
            case shaders::Type::BOX_BUTTON:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pBoxButtonFragmentShader));
                break;
            case shaders::Type::SENSOR:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pSensorFragmentShader));
                break;
            case shaders::Type::TEXT:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pTextFragmentShader));
                break;
            case shaders::Type::IMAGE:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pImageFragmentShader));
                break;
            case shaders::Type::KEY:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pKeyFragmentShader));
                break;
            case shaders::Type::CHARACTER_KEY:
                rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pCharacterKeyFragmentShader));
                break;
			case shaders::Type::KEY_SELECTION:
				rupShader = std::unique_ptr<Shader>(new Shader(shaders::pStaticVertexShader, shaders::pKeySelectionFragmentShader));
				break;
            default:
                throwError(OperationNotifier::Operation::BUG, "Shader does not exist");
            }
            pShader = rupShader.get();
            mShaders[shader] = std::move(rupShader);
        }

        return pShader;
    }

    Mesh const * AssetManager::fetchMesh(meshes::Type mesh)
    {
        // Search in map for mesh and create if needed
        std::unique_ptr<Mesh>& rupMesh = mMeshes[mesh];
        Mesh* pMesh = rupMesh.get();

        if (pMesh == NULL)
        {
            switch (mesh)
            {
            case meshes::Type::QUAD:
                rupMesh = std::unique_ptr<Mesh>(new Mesh(&meshes::quadVertices, &meshes::quadTextureCoordinates));
                break;
            case meshes::Type::LINE:
                rupMesh = std::unique_ptr<Mesh>(new Mesh(&meshes::lineVertices, &meshes::lineTextureCoordinates));
                break;
            default:
                throwError(OperationNotifier::Operation::BUG, "Mesh does not exist");
            }
            pMesh = rupMesh.get();
            mMeshes[mesh] = std::move(rupMesh);
        }

        return pMesh;
    }

    Font const * AssetManager::fetchFont(std::string filepath)
    {
        // Search for font
        std::unique_ptr<Font>& rupFont = mFonts[filepath];
        Font* pFont = rupFont.get();

        // Create font if not existing
        if (pFont == NULL)
        {
            // Check whether empty font must be created
            if (filepath == EMPTY_STRING_ATTRIBUTE)
            {
                // Empty font
                rupFont = std::unique_ptr<Font>(
                    new EmptyFont());

                // Tell the user about this
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No default font set. Text rendering will not work");
            }
            else
            {
                // Check filepath
                if (!checkFileNameExtension(filepath, "ttf"))
                {
                    throwWarning(OperationNotifier::Operation::FONT_LOADING, "Font file has unknown format", filepath);
                }

                // Load font to FreeType face
                std::unique_ptr<FT_Face> upFace = std::unique_ptr<FT_Face>(new FT_Face);
                if (FT_New_Face(mFreeTypeLibrary, buildPath(filepath).c_str(), 0, upFace.get()))
                {
                    throwError(OperationNotifier::Operation::FONT_LOADING, "Failed to load font with FreeType Library", filepath);
                }
                else
                {
					// TODO: instead of adding more and more letters, one might think about a dynamic buffer approach instead of an static atlas

                    // Decide character set
					std::map<char16_t, CharacterDirection> characters;

					// Add basic characters
					characters.insert(charsets::BASIC.begin(), charsets::BASIC.end());

					// Add further characters
					characters.insert(charsets::GERMANY_GERMAN.begin(), charsets::GERMANY_GERMAN.end()); // German
					characters.insert(charsets::ISRAEL_HEBREW.begin(), charsets::ISRAEL_HEBREW.end()); // Hebrew

                    // Give face to a font object (it will delete it in the end)
                    rupFont = std::unique_ptr<Font>(
                        new AtlasFont(
                            mpGUI,
                            filepath,
                            std::move(upFace),
                            characters));
                }
            }

            // Get raw pointer to font
            pFont = rupFont.get();

            // Add font to own map
            mFonts[filepath] = std::move(rupFont);
        }

        return pFont;
    }

    void AssetManager::resizeFontAtlases()
    {
        for (auto& rPair : mFonts)
        {
            rPair.second->resizeFontAtlases();
        }
    }

	void AssetManager::playSound(std::string filepath)
	{
		// Search in map for sounds and create if needed
		std::unique_ptr<Audio>& rupSound = mSounds[filepath];
		Audio* pSound = rupSound.get();

		if (pSound == NULL)
		{
			// Check filepath
			if (!checkFileNameExtension(filepath, "ogg"))
			{
				throwWarning(OperationNotifier::Operation::AUDIO_LOADING, "Audio file has unknown format", filepath);
			}

			// Try to read audio file from disk
			int sampleCount, channelCount, sampleRate = 0;
			short* pBuffer = NULL;
			sampleCount = stb_vorbis_decode_filename(buildPath(filepath).c_str(), &channelCount, &sampleRate, &pBuffer);
			if (sampleCount < 0)
			{
				throwWarning(OperationNotifier::Operation::AUDIO_LOADING, "Failed to load audio file", filepath);
				return;
			}

			// Fill sound audio structure (buffer will be freed at destruction of that pointer)
			rupSound = std::unique_ptr<Audio>(
				new Audio(
					channelCount,
					sampleCount,
					sampleRate,
					pBuffer));

			// Store sound
			pSound = rupSound.get();
			mSounds[filepath] = std::move(rupSound);
		}

		// Check for PortAudio
		if (mPortAudioInitialized)
		{
			// Variable to fetch errors
			PaError err;

			// Stop existing stream if necessary
			if (mpStream != NULL)
			{
				// Aborts stream (stop would wait until buffer finished)
				err = Pa_AbortStream(mpStream);
				if (err != paNoError)
				{
					OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
					mpStream = NULL;
					return;
				}

				// Close stream
				err = Pa_CloseStream(mpStream);
				if (err != paNoError)
				{
					OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
					mpStream = NULL;
					return;
				}

				// Set stream to NULL
				mpStream = NULL;
			}

			// Set value of static structure accessed by the callbacks
			mupAudioOutput = std::unique_ptr<AudioOutput>(new AudioOutput(pSound));

			// Open new stream
			PaStreamParameters parameters;
			parameters.device = Pa_GetDefaultOutputDevice();
			parameters.channelCount = pSound->getChannelCount();
			parameters.sampleFormat = paInt16;
			parameters.suggestedLatency = Pa_GetDeviceInfo(parameters.device)->defaultLowOutputLatency;
			parameters.hostApiSpecificStreamInfo = NULL;
			if (parameters.device == paNoDevice)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: Have not found an audio device");
				return;
			}

			// Open stream
			err = Pa_OpenStream(
				&mpStream,
				NULL,
				&parameters,
				pSound->getSampleRate(),
				paFramesPerBufferUnspecified,
				paClipOff,
				audioStreamUpdateCallback,
				mupAudioOutput.get()); // provide pointer to audio output data (must be valid during callback)
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
				mpStream = NULL;
				return;
			}

			// Start stream
			err = Pa_StartStream(mpStream);
			if (err != paNoError)
			{
				OperationNotifier::notifyAboutWarning(OperationNotifier::Operation::RUNTIME, "PortAudio error: " + std::string(Pa_GetErrorText(err)));
				mpStream = NULL; // possible memory leak
				return;
			}
		}
	}

    std::unique_ptr<TextFlow> AssetManager::createTextFlow(
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        float scale,
        std::u16string content,
		bool overflowHeight,
		bool collapseSpaces)
    {
        return std::move(
            std::unique_ptr<TextFlow>(
                new TextFlow(
                    mpGUI,
                    this,
                    mpGUI->getDefaultFont(),
                    fontSize,
                    alignment,
                    verticalAlignment,
                    scale,
                    content,
					overflowHeight,
					collapseSpaces)));
    }

    std::unique_ptr<TextSimple> AssetManager::createTextSimple(
        FontSize fontSize,
        float scale,
        std::u16string content)
    {
        return std::move(
            std::unique_ptr<TextSimple>(
                new TextSimple(
                    mpGUI,
                    this,
                    mpGUI->getDefaultFont(),
                    fontSize,
                    scale,
                    content)));
    }

    std::unique_ptr<Key> AssetManager::createKey(Layout const * pLayout, char16_t character)
    {
        return std::move(
            std::unique_ptr<Key>(
                new CharacterKey(
                    pLayout,
                    this,
                    mpGUI->getDefaultFont(),
                    character)));
    }

    std::unique_ptr<Image> AssetManager::createImage(Layout const * pLayout, std::string filepath, ImageAlignment alignment)
    {
        return std::move(
            std::unique_ptr<Image>(
                new Image(
                    pLayout,
                    this,
                    filepath,
                    alignment)));
    }
}
