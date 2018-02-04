//	The MIT License (MIT)
//
//	Copyright(c) 2017 Raphael Menges
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in all
//	copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
//	SOFTWARE.

/*!
 *  \brief     Interface to access eyeGUI functions.
 *  \details   This interface provides multiply functions and abstract class declarations to create, render and manipulate the eyeGUI user interface.
 *  \author    Raphael Menges
 *  \version   0.9
 *  \license   This project is released under the MIT License (MIT)
 */

#ifndef EYE_GUI_H_
#define EYE_GUI_H_

#include <string>
#include <memory>
#include <map>
#include <functional>
#include <vector>

//! Namespace of eyeGUI interface.
namespace eyegui
{
	class GUI;
	class Layout;
	class Frame;

	//! Namespace of styling properties
	namespace property
	{
		//! Enumeration of durations, which are stored as floats.
		enum class Duration
		{
			AnimationDuration,
			SensorPenetrationIncreaseDuration,
			SensorPenetrationDecreaseDuration,
			ButtonThresholdIncreaseDuration,
			ButtonThresholdDecreaseDuration,
			ButtonPressingDuration,
			DimIncreaseDuration,
			DimDecreaseDuration,
			AdaptiveScaleIncreaseDuration,
			AdaptiveScaleDecreaseDuration,
			FlashDuration,
			KeyboardKeySelectionDuration,

			// Experimental
			FutureKeyboardPressDuration,
			FutureKeyboardThresholdDuration,
		};

		//! Enumeration of percentages, which are stored as floats.
		enum class Percentage
		{
			DimAlpha
		};

		//! Enumeration of amounts, which are stored as floats.
		enum class Amount
		{
			SensorInteractionPenetrationAmount,
			KeyboardZoomSpeedMultiplier,
			FlowSpeedMultiplier,
			TextEditScrollSpeedMultiplier,
			MaximalAdaptiveScaleIncrease,

			// Experimental
			FutureKeyboardRepeatKeyThresholdMultiplier,
			FutureKeyboardSpaceKeyThresholdMultiplier,
			FutureKeyboardBackspaceKeyThresholdMultiplier,
			FutureKeyboardSuggestionLineThresholdMultiplier,
			FutureKeyboardRetriggerDelay
		};

		//! Enumeration of colors, which are stored as vec4.
		enum class Color
		{
			Color,
			BackgroundColor,
			HighlightColor,
			SeparatorColor,
			SelectionColor,
			IconColor,
			FontColor,
			DimColor,
			FlashColor,
			MarkColor,
			PickColor,
			ThresholdColor
		};

		//! Enumeration of asset paths, which are stored as strings.
		enum class AssetPath
		{
			ButtonHitSound,
			ButtonDownSound,
			KeyPressSound
		};
	}

	//! Enumeration of possible character sets for font rendering.
	enum class KeyboardLayout { US_ENGLISH, GERMANY_GERMAN, ISRAEL_HEBREW, GREECE_GREEK };

	//! Enumeration of possible font sizes. Size of keyboard font cannot be set in GUIBuilder.
	enum class FontSize { TALL, MEDIUM, SMALL, KEYBOARD };

	//! Enumeration of possible text flow alignments. Natural aligns left for left to right text and right for right to left text
	enum class TextFlowAlignment { LEFT, RIGHT, CENTER, JUSTIFY, NATURAL };

	//! Enumeration of possible vertical text flow alignments
	enum class TextFlowVerticalAlignment { TOP, CENTER, BOTTOM };

	//! Enumeration of possible image alignments.
	enum class ImageAlignment { ORIGINAL, STRETCHED, ZOOMED };

	//! Enumeration of cases of keyboard.
	enum class KeyboardCase { LOWER, UPPER };

	//! Enumeration of available color formats.
	enum class ColorFormat { RGBA, BGRA };

	//! Enumeration of available description visibility behaviors.
	enum class DescriptionVisibility { HIDDEN, ON_PENETRATION, VISIBLE };

	//! Struct used by drift map to compensate for eye tracker accuracy issues.
	struct DriftGrid
	{
		//! Horizontal cell count.
		static const int RES_X = 16;

		//! Vertical cell count.
		static const int RES_Y = 9;

		//! Vertices of grid holding the drift values.
		std::pair<float, float> verts[RES_X + 1][RES_Y + 1] = { std::make_pair(0.f, 0.f) };
	};

	//! Abstract listener class for buttons.
	class ButtonListener
	{
	public:

		//! Constructor.
		ButtonListener();

		//! Destructor.
		virtual ~ButtonListener() = 0;

		//! Callback for hitting of button.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the button which causes the callback.
		*/
		void virtual hit(Layout* pLayout, std::string id) = 0;

		//! Callback for pushing button down.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the button which causes the callback.
		*/
		void virtual down(Layout* pLayout, std::string id) = 0;

		//! Callback for pulling button up.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the button which causes the callback.
		*/
		void virtual up(Layout* pLayout, std::string id) = 0;

		//! Callback for selecting button.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the button which causes the callback.
		*/
		void virtual selected(Layout* pLayout, std::string id) = 0;
	};

	//! Abstract listener class for sensors.
	class SensorListener
	{
	public:

		//! Constructor.
		SensorListener();

		//! Destructor.
		virtual ~SensorListener() = 0;

		//! Callback for penetration of sensor.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the sensor which causes the callback.
		  \param amount is the value of penetration at time of callback.
		*/
		void virtual penetrated(Layout* pLayout, std::string id, float amount) = 0;
	};

	//! Abstract listener class for keyboards.
	class KeyboardListener
	{
	public:

		//! Constructor.
		KeyboardListener();

		//! Destructor.
		virtual ~KeyboardListener() = 0;

		//! Callback for selecting keys of keyboard.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the keyboard which causes the callback.
		\param value is the u16string given by selected key.
		*/
		void virtual keySelected(Layout* pLayout, std::string id, std::u16string value) = 0;

		//! Callback for selecting keys of keyboard.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the keyboard which causes the callback.
		\param value is the string given by selected key.
		*/
		void virtual keySelected(Layout* pLayout, std::string id, std::string value) = 0;

		//! Callback for pressing keys of keyboard.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the keyboard which causes the callback.
		  \param value is the u16string given by pressed key.
		*/
		void virtual keyPressed(Layout* pLayout, std::string id, std::u16string value) = 0;

		//! Callback for pressing keys of keyboard.
		/*!
		  \param pLayout pointer to layout from which callback is coming.
		  \param id is the unique id of the keyboard which causes the callback.
		  \param value is the string given by pressed key.
		*/
		void virtual keyPressed(Layout* pLayout, std::string id, std::string value) = 0;
	};

	//! Abstract listener class for word suggest.
	class WordSuggestListener
	{
	public:

		//! Constructor.
		WordSuggestListener();

		//! Destructor.
		virtual ~WordSuggestListener() = 0;

		//! Callback for choosing suggested word.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the word suggest which causes the callback.
		\param value is the u16string of the suggestion.
		*/
		void virtual chosen(Layout* pLayout, std::string id, std::u16string value) = 0;

		//! Callback for choosing suggested word.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the word suggest which causes the callback.
		\param value is the string of the suggestion.
		*/
		void virtual chosen(Layout* pLayout, std::string id, std::string value) = 0;
	};

	//! Struct for relative values of position and size.
	struct RelativePositionAndSize
	{
		float x = 0;
		float y = 0;
		float width = 0;
		float height = 0;
	};

	//! Struct for absolute pixel values of position and size.
	struct AbsolutePositionAndSize
	{
		int x = 0;
		int y = 0;
		int width = 0;
		int height = 0;
	};

	//! Class for holding audio records.
	class AudioRecord
	{
	public:

		//! Constructor.
		AudioRecord(unsigned int channelCount, unsigned int sampleRate, unsigned int maxSeconds);

		//! Add sample. Increments index and returns whether successful.
		bool addSample(short sample);

		//! Getter.
		int getChannelCount() const;
		int getSampleRate() const;
		int getSampleCount() const;
		std::weak_ptr<const std::vector<short> > getBuffer() const;

	private:

		//! Members.
		std::shared_ptr<std::vector<short> > mspBuffer;
		unsigned int mChannelCount;
		unsigned int mSampleRate;
		unsigned int mMaxSeconds;
		unsigned int mIndex = 0;
	};

	//! Struct for input. Coordinate system orginates at upper left corner of screen.
	struct Input
	{
		int gazeX = 0; //!< X position of gaze in pixels
		int gazeY = 0; //!< Y position of gaze in pixels
		bool gazeUsed = false; //!< Indicates usage of gaze by eyeGUI
		bool instantInteraction = false; //!< Instant interaction with element beneath gaze
	};

	//! Builder for GUI
	class GUIBuilder
	{
	public:

		GUI* construct() const; //!< Returns pointer to built GUI
		int width = 1280; //!< Width of GUI as integer
		int height = 720; //!< Height of GUI as integer
		std::string fontFilepath = ""; //!< FontFilepath is path to a .ttf font file
		KeyboardLayout keyboardLayout = KeyboardLayout::US_ENGLISH; //!< Keyboard layout with which keyboard are initialized
		std::string localizationFilepath = ""; //!< LocalizationFilepath is path to a .leyegui file
		float vectorGraphicsDPI = 96.0f; //!< Dpi which are used to rasterize vector graphics
		float fontTallSize = 0.1f; //!< Height of tall font in percentage of GUI height
		float fontMediumSize = 0.04f; //!< Height of medium font in percentage of GUI height
		float fontSmallSize = 0.0175f; //!< Height of small font in percentage of GUI height
		FontSize descriptionFontSize = FontSize::SMALL; //!< Font size of icon element descriptions
		bool resizeInvisibleLayouts = true; //!< Resize invisible layouts. Has advantage that one can ask for the size of elements all time
		bool useDriftMap = true; //!< Use estimations from drift map to improve accuracy
	};

	//! Creates layout inside GUI and returns pointer to it. Is executed at update call.
	/*!
	  \param pGUI pointer to GUI.
	  \param filepath is path to layout xml file.
	  \param layer is index of layer into which layout is added. Higher ones are in front.
	  \param visible shall added layout be visible.
	  \return pointer to added layout. Null if creation was not possible because layouts were locked.
	*/
	Layout* addLayout(GUI* pGUI, std::string filepath, int layer = 0, bool visible = true);

	//! Removes layout in GUI by pointer. Is executed at update call.
	/*!
	  \param pGUI pointer to GUI.
	  \param pLayout is the pointer to layout which should be removed.
	*/
	void removeLayout(GUI* pGUI, Layout const * pLayout);

	//! Simple update of whole GUI. Should not be used as default call, only thought for resizing added layouts or applying of settings.
	/*!
	\param pGUI pointer to GUI.
	*/
	void updateGUI(GUI* pGUI);

	//! Update whole GUI.
	/*!
	  \param pGUI pointer to GUI.
	  \param tpf passed time since last rendering in seconds as float.
	  \param input struct.
	  \return input struct with information about usage.
	*/
	Input updateGUI(GUI* pGUI, float tpf, const Input input);

	//! Draw whole GUI.
	/*!
	  \param pGUI pointer to GUI.
	*/
	void drawGUI(GUI const * pGUI);

	//! Terminate GUI.
	/*!
	  \param pGUI pointer to GUI which should be terminated.
	*/
	void terminateGUI(GUI* pGUI);

	//! Resize GUI. Is executed at update call.
	/*!
	  \param pGUI pointer to GUI.
	  \param width of GUI as integer.
	  \param height of GUI as integer.
	*/
	void resizeGUI(GUI* pGUI, int width, int height);

	//! Load stylesheet. Is directly executed.
	/*!
	  \param pGUI pointer to GUI.
	  \param filepath is path to stylesheet file.
	*/
	void loadStyleSheet(GUI* pGUI, std::string filepath);

	//! Set value of property in style tree class.
	/*!
	\param pGUI pointer to GUI.
	\param styleClass is name of style class where property exists.
	\param property in which value is set.
	\param value is encoded value.
	*/
	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, property::Duration property, std::string value);
	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, property::Percentage property, std::string value);
	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, property::Amount property, std::string value);
	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, property::Color property, std::string value);
	void setStyleTreePropertyValue(GUI* pGUI, std::string styleClass, property::AssetPath property, std::string value);

	//! Set gaze visualization drawing.
	/*!
	  \param pGUI pointer to GUI.
	  \param draw indicates whether gaze visualization should be drawn.
	*/
	void setGazeVisualizationDrawing(GUI* pGUI, bool draw);

	//! Toggle gaze visualization drawing.
	/*!
	  \param pGUI pointer to GUI.
	*/
	void toggleGazeVisualizationDrawing(GUI* pGUI);

	//! Resets drift map (potentially after a recalibration of the eyetracker)
	/*!
		\param pGUI pointer to GUI.
	*/
	void resetDriftMap(GUI* pGUI);

	//! Retrieve copy of structure in current drift map.
	/*!
		\param pGUI pointer to GUI.
		\return Copy of drift grid of current drift map.
	*/
	DriftGrid getCurrentDriftMap(GUI const * pGUI);

	//! Applies drift correction for provided coordinate. Does also work when eyeGUI does not use the drift map itself.
	/*!
	\param pGUI pointer to GUI.
	\param rX is reference to x coordinate to correct.
	\param rY is reference to y coordinate to correct.
	*/
	void applyDriftMap(GUI const * pGUI, float &rX, float &rY);

	//! Set how descriptions of icon elements are displayed.
	/*!
	\param pGUI pointer to GUI.
	\param visbility describes the visibility of descriptions in that GUI.
	*/
	void setDescriptionVisibility(GUI* pGUI, DescriptionVisibility visbility);

	//! Prefetch image to avoid lags.
	/*!
	  \param pGUI pointer to GUI.
	  \param filepath is path to image which should be prefetched.
	*/
	void prefetchImage(GUI* pGUI, std::string filepath);

	//! Fetch localization from localization file by key.
	/*!
	\param pGUI pointer to GUI.
	\param key defines where to look up in localization file.
	\return localization if available. Empty string else.
	*/
	std::u16string fetchLocalization(GUI const * pGUI, std::string key);

	//! Add dictionary which can be used for text suggestions.
	/*!
	  \param pGUI pointer to GUI.
	  \param filepath is path to dictionary file with words to add.
	  \return Handle to access dictionary via interface.
	*/
	unsigned int addDictionary(GUI* pGUI, std::string filepath);

	//! Move layout to front. Is executed at update call.
	/*!
	  \param pGUI pointer to GUI.
	  \param pLayout pointer to layout.
	*/
	void moveLayoutToFront(GUI* pGUI, Layout* pLayout);

	//! Move layout to back. Is executed at update call.
	/*!
	  \param pGUI pointer to GUI.
	  \param pLayout pointer to layout.
	*/
	void moveLayoutToBack(GUI* pGUI, Layout* pLayout);

	//! Set global layout of keyboard.
	/*!
	\param pGUI pointer to GUI.
	\param keyboardLayout is layout of keyboard.
	*/
	void setKeyboardLayout(GUI* pGUI, KeyboardLayout keyboardLayout);

	//! Play a sound file.
	/*!
	\param pGUI pointer to GUI.
	\param filepath to sound file.
	*/
	void playSound(GUI* pGUI, std::string filepath);

	//! Start recording of audio.
	/*!
	\param pGUI pointer to GUI.
	\return true if successfully started, else false.
	*/
	bool startAudioRecording(GUI* pGUI);

	//! End recording of audio.
	/*!
	\param pGUI pointer to GUI.
	\return true if successfully ended, else false.
	*/
	bool endAudioRecording(GUI* pGUI);

	//! Retrieve latest recorded audio. May not be called during recording.
	/*!
	\param pGUI pointer to GUI.
	\return Shared pointer to the recorded audio data. Nullptr in case no audio is available.
	*/
	std::shared_ptr<const AudioRecord> retrieveAudioRecord(GUI const * pGUI);

	//! Control layout's input usage.
	/*!
	  \param pLayout pointer to layout.
	  \param useInput indicates whether layout may use input or ignore it.
	*/
	void setInputUsageOfLayout(Layout* pLayout, bool useInput);

	//! Returns whether the visbility of provided layout is set to true.
	/*!
	\param pLayout pointer to layout.
	\return true if set to visible and false else.
	*/
	bool isLayoutVisible(Layout const * pLayout);

	//! Set visibility of layout.
	/*!
	  \param pLayout pointer to layout.
	  \param visible is a bool value to set visibility.
	  \param reset indicates whether all elements in layout should be reset.
	  \param fade indicates, whether layer should fade.
	*/
	void setVisibilityOfLayout(
		Layout* pLayout,
		bool visible,
		bool reset = false,
		bool fade = false);

	//! Getter for relative position and size of element. Values are relative in respect to layout.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return relative position and size of element. Filled with initial values if element not found.
	*/
	RelativePositionAndSize getRelativePositionAndSizeOfElement(
		Layout* pLayout,
		std::string id);

	//! Getter for absolute pixel position and size of element. Values are in pixel space of GUI.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return absolute position and size of element. Filled with initial values if element not found.
	*/
	AbsolutePositionAndSize getAbsolutePositionAndSizeOfElement(
		Layout* pLayout,
		std::string id);

	//! Set value of property in element.
	/*!
	\param pGUI pointer to GUI.
	\param id is the unique id of an element.
	\param property is value to set.
	\param value is encoded value.
	*/
	void setElementStylePropertyValue(Layout* pLayout, std::string id, property::Duration property, std::string value);
	void setElementStylePropertyValue(Layout* pLayout, std::string id, property::Percentage property, std::string value);
	void setElementStylePropertyValue(Layout* pLayout, std::string id, property::Amount property, std::string value);
	void setElementStylePropertyValue(Layout* pLayout, std::string id, property::Color property, std::string value);
	void setElementStylePropertyValue(Layout* pLayout, std::string id, property::AssetPath property, std::string value);

	//! Activity of element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param active indicates the state of acitvity.
	  \param fade indicates, whether activity should fade.
	*/
	void setElementActivity(
		Layout* pLayout,
		std::string id,
		bool active,
		bool fade = false);

	//! Toggle activity of element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param fade indicates, whether activity should fade.
	*/
	void toggleElementActivity(
		Layout* pLayout,
		std::string id,
		bool fade = false);

	//! Get activity of element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if element with given id is active and false else
	*/
	bool isElementActive(Layout const * pLayout, std::string id);

	//! Set whether element is dimming.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param dimming is the new choice.
	*/
	void setElementDimming(
		Layout* pLayout,
		std::string id,
		bool dimming);

	//! Set whether element is marking.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param marking is the new choice.
	  \param depth of children of this element, which are marked (or unmarked) too. Negative depth indicates, that all children are affected.
	*/
	void setElementMarking(
		Layout* pLayout,
		std::string id,
		bool marking,
		int depth = 0);

	//! Get whether element is dimming.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if element with given id is dimming and false else.
	*/
	bool isElementDimming(Layout const * pLayout, std::string id);

	//! Get whether element is marking.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if element with given id is marking and false else.
	*/
	bool isElementMarking(Layout const * pLayout, std::string id);

	//! Set hiding of element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param hidden should be true to hide element and false to unhide it.
	*/
	void setElementHiding(Layout* pLayout, std::string id, bool hidden);

	//! Check for existence of id.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if element with given id is found and false else
	*/
	bool checkForId(Layout const * pLayout, std::string id);

	//! Flashes element one time.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	*/
	void flash(Layout* pLayout, std::string id);

	//! Set interactive element as highlighted.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param doHighlight indicates, whether elemen with given id should be highlighted or not.
	*/
	void highlightInteractiveElement(
		Layout* pLayout,
		std::string id,
		bool doHighlight);

	//! Toggle highlighting of interactive element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	*/
	void toggleHighlightInteractiveElement(Layout* pLayout, std::string id);

	//! Check whether interactive element is highlighted.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if interactive element with given id is highlighted.
	*/
	bool isInteractiveElementHighlighted(Layout const * pLayout, std::string id);

	//! Set icon of icon element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param iconFilepath path to image which should be used as icon.
	*/
	void setIconOfIconElement(
		Layout* pLayout,
		std::string id,
		std::string iconFilepath);

	//! Set icon of icon element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param name is unique name of later texture. Can be used to overwrite existing one.
	  \param width is with of icon.
	  \param height is height of icon.
	  \param format is format of pixel data.
	  \param pIconData is pointer to unsigned char data. Must have size of width * height * channelCount (as specified in format implicitly).
	  \param flipY indicates, whether texture is flipped vertically.
	*/
	void setIconOfIconElement(
		Layout* pLayout,
		std::string id,
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pIconData,
		bool flipY = false);

	//! Set image of picture.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param name is unique name of later texture. Can be used to overwrite existing one.
	  \param width is with of image.
	  \param height is height of image.
	  \param format is format of pixel data.
	  \param pData is pointer to unsigned char data. Must have size of width * height *  channelCount (as specified in format implicitly).
	  \param flipY indicates, whether texture is flipped vertically.
	*/
	void setImageOfPicture(
		Layout* pLayout,
		std::string id,
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pData,
		bool flipY = false);

	//! Set image of picture.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param name is unique name of already loaded texture (either filepath or name of custom image)
	*/
	void setImageOfPicture(
		Layout* pLayout,
		std::string id,
		std::string name);

	//! Fetch image from disk to use it later.
	/*!
	  \param pLayout pointer to layout.
	  \param name is unique name of later texture. Can be used to overwrite existing one.
	  \param width is with of image.
	  \param height is height of image.
	  \param format is format of pixel data.
	  \param pData is pointer to unsigned char data. Must have size of width * height *  channelCount (as specified in format implicitly).
	  \param flipY indicates, whether texture is flipped vertically.
	*/
	void fetchImage(
		Layout* pLayout,
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pData,
		bool flipY = false);

	//! Interact with interactive element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	*/
	void interactWithInteractiveElement(Layout* pLayout, std::string id);

	//! Select interactive element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	*/
	void selectInteractiveElement(Layout* pLayout, std::string id);

	//! Deselect currently selected element.
	/*!
	  \param pLayout pointer to layout.
	*/
	void deselectInteractiveElement(Layout* pLayout);

	//! Interact with currently selected interactive element.
	/*!
	  \param pLayout pointer to layout.
	*/
	void interactWithSelectedInteractiveElement(Layout* pLayout);

	//! Select next interactive element, returns whether reached end of layout. If so, nothing is selected.
	/*!
	  \param pLayout pointer to layout.
	*/
	bool selectNextInteractiveElement(Layout* pLayout);

	//! Hit button.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	*/
	void hitButton(Layout* pLayout, std::string id);

	//! Classify currently selected button.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param accept if true and reject selection when false.
	*/
	void classifyButton(Layout* pLayout, std::string id, bool accept);

	//! Button down.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param immediately indicates, whether animation is skipped or not.
	*/
	void buttonDown(Layout* pLayout, std::string id, bool immediately = false);

	//! Button up.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param immediately indicates, whether animation is skipped or not.
	*/
	void buttonUp(Layout* pLayout, std::string id, bool immediately = false);

	//! Is button a switch?
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return true if element found by id exists, is a button and switch. Else false.
	*/
	bool isButtonSwitch(Layout const * pLayout, std::string id);

	//! Penetrate sensor.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param amount is value of peneteration.
	*/
	void penetrateSensor(Layout* pLayout, std::string id, float amount);

	//! Set content of text block. Works only if no key is used for localization.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param content is new content for text block as UTF-16 string.
	*/
	void setContentOfTextBlock(Layout* pLayout, std::string id, std::u16string content);

	//! Set content of text block. Works only if no key is used for localization.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param content is new content for text block as UTF-8 string.
	*/
	void setContentOfTextBlock(Layout* pLayout, std::string id, std::string content);

	//! Set key of text block. Works only if used localization file includes key.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param key is new key for text block.
	*/
	void setKeyOfTextBlock(Layout* pLayout, std::string id, std::string key);

	//! Set fast typing for keyboard.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param useFastTyping indicates, whether fast typing should be used or not.
	*/
	void setFastTypingOfKeyboard(Layout* pLayout, std::string id, bool useFastTyping);

	//! Set case of letters in keyboard.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param case indicates case of displayed letters.
	*/
	void setCaseOfKeyboard(Layout* pLayout, std::string id, KeyboardCase keyboardCase);

	//! Get count of available keymaps in keyboard.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return count of keymaps
	*/
	unsigned int getCountOfKeymapsInKeyboard(Layout const * pLayout, std::string id);

	//! Set keymap of keyboard by index.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param keymapIndex is index of keymap.
	*/
	void setKeymapOfKeyboard(Layout* pLayout, std::string id, unsigned int keymapIndex);

	//! Classify currently selected key on keyboard. If no key selected, nothing happens.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param accept if true and reject selection when false.
	*/
	void classifyKey(Layout* pLayout, std::string id, bool accept);

	//! Give input to word suggest element.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param input is input for suggestions as UTF-16 string.
	\param dictionaryIndex is index of used dictionary.
	*/
	void suggestWords(Layout* pLayout, std::string id, std::u16string input, unsigned int dictionaryIndex);

	//! Give input to word suggest element.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param input is input for suggestions as UTF-8 string.
		\param dictionaryIndex is index of used dictionary.
	*/
	void suggestWords(Layout* pLayout, std::string id, std::string input, unsigned int dictionaryIndex);

	//! Give input to word suggest element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param input is input for suggestions as UTF-16 string.
	  \param dictionaryIndex is index of used dictionary.
	  \param rBestSuggestion is reference to UTF-16 string into which best suggestion is written. May be empty.
	*/
	void suggestWords(Layout* pLayout, std::string id, std::u16string input, unsigned int dictionaryIndex, std::u16string& rBestSuggestion);

	//! Give input to word suggest element.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param input is input for suggestions as UTF-8 string.
	  \param dictionaryIndex is index of used dictionary.
	  \param rBestSuggestion is reference to UTF-8 string into which best suggestion is written. May be empty.
	*/
	void suggestWords(Layout* pLayout, std::string id, std::string input, unsigned int dictionaryIndex, std::string& rBestSuggestion);

	//! Clears suggestions of word suggest element.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
	*/
	void clearSuggestions(Layout* pLayout, std::string id);

	//! Set space of flow element.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param space is new space in percent of width or height, depending on direction.
	*/
	void setSpaceOfFlow(Layout* pLayout, std::string id, float space);

	//! Set progress in progress bar.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param progress is progress between 0 and 1 which is to be set.
	*/
	void setProgress(Layout* pLayout, std::string id, float progress);

	//! Move cursor over given amount of letters in text edit element. If content is exceeded, cursor is placed at last valid position. Whitespace is skipped.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param letterCount indicates how far cursor is moved. Negative values result in a leftward movement.
	*/
	void moveCursorOverLettersInTextEdit(Layout* pLayout, std::string id, int letterCount);

	//! Move cursor over given amount of words in text edit element. If content is exceeded, cursor is placed at last valid position.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param wordCount indicates how far cursor is moved. Negative values result in a leftward movement.
	*/
	void moveCursorOverWordsInTextEdit(Layout* pLayout, std::string id, int wordCount);

	//! Move cursor to start of text in text edit element.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	*/
	void moveCursorToStartInTextEdit(Layout* pLayout, std::string id);

	//! Move cursor to end of text in text edit element.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	*/
	void moveCursorToEndInTextEdit(Layout* pLayout, std::string id);

	//! Add text at cursor in text edit element.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param content is added text.
	*/
	void addContentAtCursorInTextEdit(Layout* pLayout, std::string id, std::u16string content);

	//! Set content of text edit.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param content is the set text.
	*/
	void setContentOfTextEdit(Layout* pLayout, std::string id, std::u16string content);

	//! Get content of text edit.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\return content of the text edit. Is empty if element not found.
	*/
	std::u16string getContentOfTextEdit(Layout const * pLayout, std::string id);

	//! Set content in text edit.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param letterCount is count of deleted letters. Positive values indicates rightward deletion, negative values leftward deletion.
	*/
	void deleteContentAtCursorInTextEdit(Layout* pLayout, std::string id, int letterCount);

	//! Get content of active entity in text edit.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\return string of active entity or empty string if there is no active entity.
	*/
	std::u16string getActiveEntityContentInTextEdit(Layout const * pLayout, std::string id);

	//! Set content of active entity in text edit.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param content that is used to replace active entity.
	*/
	void setActiveEntityContentInTextEdit(Layout* pLayout, std::string id, std::u16string content);

	//! Add brick to stack
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param filepath is relative path to brick file.
	*/
	void addBrickToStack(
		Layout* pLayout,
		std::string id,
		std::string filepath);

	//! Add brick to stack
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param filepath is relative path to brick file.
	\param idMapper changes ids inside brick to ones in map.
	*/
	void addBrickToStack(
		Layout* pLayout,
		std::string id,
		std::string filepath,
		std::map<std::string, std::string> idMapper);

	//! Register listener to button.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param wpListener is weak pointer to listener that should be registered.
	*/
	void registerButtonListener(
		Layout* pLayout,
		std::string id,
		std::weak_ptr<ButtonListener> wpListener);

	//! Register listener to sensor.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param wpListener is weak pointer to listener that should be registered.
	*/
	void registerSensorListener(
		Layout* pLayout,
		std::string id,
		std::weak_ptr<SensorListener> wpListener);

	//! Register listener to keyboard.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param wpListener is weak pointer to listener that should be registered.
	*/
	void registerKeyboardListener(
		Layout* pLayout,
		std::string id,
		std::weak_ptr<KeyboardListener> wpListener);

	//! Register listener to word suggest.
	/*!
		\param pLayout pointer to layout.
		\param id is the unique id of an element.
		\param wpListener is weak pointer to listener that should be registered.
	*/
	void registerWordSuggestListener(
		Layout* pLayout,
		std::string id,
		std::weak_ptr<WordSuggestListener> wpListener);

	//! Replace element with brick.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param filepath is path to brick xml file.
	  \param fade indicates, whether replaced element should fade.
	*/
	void replaceElementWithBrick(
		Layout* pLayout,
		std::string id,
		std::string filepath,
		bool fade = false);

	//! Replace element with brick.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \param filepath is path to brick xml file.
	  \param idMapper changes ids inside brick to ones in map.
	  \param fade indicates, whether replaced element should fade.
	*/
	void replaceElementWithBrick(
		Layout* pLayout,
		std::string id,
		std::string filepath,
		std::map<std::string, std::string> idMapper,
		bool fade = false);

	//! Creates floating frame with brick inside
	/*!
	  \param pLayout pointer to layout.
	  \param filepath is path to brick xml file.
	  \param relativePositionX initial relative x position.
	  \param relativePositionY initial relative y position.
	  \param relativeSizeX initial relative x size.
	  \param relativeSizeY initial relative y size.
	  \param visible indicates, whether frame should be visible or not.
	  \param fade indicates, whether frame should fade in.
	  \return index of created floating frame.
	*/
	unsigned int addFloatingFrameWithBrick(
		Layout* pLayout,
		std::string filepath,
		float relativePositionX,
		float relativePositionY,
		float relativeSizeX,
		float relativeSizeY,
		bool visible = true,
		bool fade = false);

	//! Creates floating frame with brick inside
	/*!
	  \param pLayout pointer to layout.
	  \param filepath is path to brick xml file.
	  \param relativePositionX initial relative x position.
	  \param relativePositionY initial relative y position.
	  \param relativeSizeX initial relative x size.
	  \param relativeSizeY initial relative y size.
	  \param idMapper changes ids inside brick to ones in map.
	  \param visible indicates, whether frame should be visible or not.
	  \param fade indicates, whether frame should fade in.
	  \return index of created floating frame.
	*/
	unsigned int addFloatingFrameWithBrick(
		Layout* pLayout,
		std::string filepath,
		float relativePositionX,
		float relativePositionY,
		float relativeSizeX,
		float relativeSizeY,
		std::map<std::string, std::string> idMapper,
		bool visible = true,
		bool fade = false);

	//! Set visibility of floating frame.
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param visible is a bool value to set visibility.
	  \param reset indicates whether all elements in layout should be reset.
	  \param fade indicates, whether frame should fade.
	*/
	void setVisibilityOFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		bool visible,
		bool reset = false,
		bool fade = false);

	//! Removes floating frame from layout.
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param fade indicates, whether floating frame should fade out.
	*/
	void removeFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		bool fade = false);

	//! Translates floating frame
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param translateX amount of translation in x direction.
	  \param translateY amount of translation in y direction.
	*/
	void translateFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		float translateX,
		float translateY);

	//! Scales floating frame
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param scaleX scaling in x direction.
	  \param scaleY scaling in y direction.
	*/
	void scaleFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		float scaleX,
		float scaleY);

	//! Set relative position of floating frame
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param relativePositionX relative x position.
	  \param relativePositionY relative y position.
	*/
	void setPositionOfFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		float relativePositionX,
		float relativePositionY);

	//! Set relative size of floating frame
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \param relativeSizeX relative x size.
	  \param relativeSizeY relative y size.
	*/
	void setSizeOfFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex,
		float relativeSizeX,
		float relativeSizeY);

	//! Move frame to front.
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	*/
	void moveFloatingFrameToFront(Layout* pLayout, unsigned int frameIndex);

	//! Move frame to back.
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	*/
	void moveFloatingFrameToBack(Layout* pLayout, unsigned int frameIndex);

	//! Getter for relative position and size of floating frame. Values are relative in respect to layout.
	/*!
	  \param pLayout pointer to layout.
	  \param frameIndex index of frame in layout.
	  \return relative position and size of floating frame. Filled with initial values if not found.
	*/
	RelativePositionAndSize getRelativePositionAndSizeOfFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex);

	//! Getter for absolute pixel position and size of floating frame. Values are in pixel space of GUI.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return absolute position and size of floating frame. Filled with initial values if not found.
	*/
	AbsolutePositionAndSize getAbsolutePositionAndSizeOfFloatingFrame(
		Layout* pLayout,
		unsigned int frameIndex);

	//! Set error callback function.
	/*!
	  \param callbackFunction is function object which should be called back.
	*/
	void setErrorCallback(std::function<void(std::string)> callbackFunction);

	//! Set warning callback function.
	/*!
	  \param callbackFunction is function object which should be called back.
	*/
	void setWarningCallback(std::function<void(std::string)> callbackFunction);

	//! Set interaction callback function. Could look like that:
	//! interactionCallback(
	//!     std::string layout,
	//!     std::string gazeCoordinate,
	//!     std::string elementType,
	//!     std::string elementId,
	//!     std::string elementRect, // "x1, y1; x2, y2" -> upper left and lower right point
	//!     std::string elementActivity,
	//!     std::string interactionType,
	//!     std::string interactionInfoA)
	/*!
	  \param callbackFunction is function object which should be called back.
	*/
	void setInteractionCallback(
		std::function<
		void(
			std::string,
			std::string,
			std::string,
			std::string,
			std::string,
			std::string,
			std::string,
			std::string)> callbackFunction);

	//! Set resize callback function since eyeGUI does not resize directly at resizeGUI call.
	/*!
	\param pGUI is pointer to GUI object which shall use callback.
	\param callbackFunction is function object which should be called back.
	*/
	void setResizeCallback(GUI* pGUI, std::function<void(int, int)> callbackFunction);

	//! Return string describing the version of the linked library.
	/*!
	  \return version given as string.
	*/
	std::string getLibraryVersion();

	//! Root filepath is the prefix used globally for ALL filepaths as prefix.
	/*!
	  \param rootFilepath is used as prefix for used filepaths.
	*/
	void setRootFilepath(std::string rootFilepath);
}

//! Namespace for simple helper functions.
namespace eyegui_helper
{
	//! Converts std::string to std::u16string and returns, whether successful.
	/*!
	\param rInput is input string.
	\param rOutput is reference to output string.
	\return TRUE if successful, FALSE otherwise.
	*/
	bool convertUTF8ToUTF16(const std::string& rInput, std::u16string& rOutput);

	//! Converts std::u16string to std::string and returns, whether successful.
	/*!
	\param rInput is input string.
	\param rOutput is reference to output string.
	\return TRUE if successful, FALSE otherwise.
	*/
	bool convertUTF16ToUTF8(const std::u16string& rInput, std::string& rOutput);

	//! Imports image and returns vector with data.
	/*!
	\param filepath to image file.
	\param rData is output data as vector of unsigned char.
	\param rWidth is width of image.
	\param rHeight is height of image.
	\param rChannelCount is channel count of image.
	\param flipY indicates whether image should be flipped in y-Direction at loading.
	\return TRUE if successful, FALSE otherwise.
	*/
	bool loadImage(
		std::string filepath,
		std::vector<unsigned char>& rData,
		int& rWidth,
		int& rHeight,
		int& rChannelCount,
		bool flipY = false);
}

//! Namespace for experiments.
namespace eyegui_experimental
{
    //! Abstract callback class for future keyboard.
    class FutureKeyboardListener
    {
    public:

        //! Constructor.
		FutureKeyboardListener();

        //! Destructor.
        virtual ~FutureKeyboardListener() = 0;

        //! Callback from suggestion line which needs new suggestions.
        /*!
          \param pLayout pointer to layout from which callback is coming.
          \param id is the unique id of the future keyboard which causes the callback.
        */
        void virtual lineSuggestions(eyegui::Layout* pLayout, std::string id) = 0;

		//! Callback from key which needs new suggestion.
		/*!
		\param pLayout pointer to layout from which callback is coming.
		\param id is the unique id of the future keyboard which causes the callback.
		\param keyId is the id of the future key which causes the callback.
		\param letter is letter(s) which would be added by key.
		*/
		void virtual keySuggestion(eyegui::Layout* pLayout, std::string id, std::string keyId, std::u16string letter) = 0;
    };

    //! Register callback for future keyboard.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param wpListener is weak pointer to callback that should be registered.
    */
    void registerFutureKeyboardListener(
        eyegui::Layout* pLayout,
        std::string id,
        std::weak_ptr<FutureKeyboardListener> wpListener);

	//! Set suggestions in line.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param suggestionA is the first suggestion which should be displayed.
	\param suggestionB is the second suggestion which should be displayed.
	\param suggestionC is the third suggestion which should be displayed.
	*/
	void setFutureKeyboardLineSuggestions(
		eyegui::Layout* pLayout,
		std::string id,
		std::u16string suggestionA,
		std::u16string suggestionB,
		std::u16string suggestionC);

    //! Set suggestion on future key.
    /*!
	  \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param keyId is id of key in keyboard.
      \param suggestion is the suggestion which should be displayed by key.
    */
    void setFutureKeySuggestion(
        eyegui::Layout* pLayout,
        std::string id,
        std::string keyId,
        std::u16string suggestion);

	//! Getter for content of future keyboard.
	/*!
	  \param pLayout pointer to layout.
	  \param id is the unique id of an element.
	  \return content or empty string if keyboard not found.
	*/
	std::u16string getFutureKeyboardContent(
		eyegui::Layout const * pLayout,
		std::string id);

	//! Clears current input and set next sentence.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	\param sentence is sentence which is predisplayed.
	*/
	void nextFutureKeyboardSentence(
		eyegui::Layout* pLayout,
		std::string id,
		std::u16string sentence);

	//! Clear predisplay of sentence to type in.
	/*!
	\param pLayout pointer to layout.
	\param id is the unique id of an element.
	*/
	void clearFutureKeyboardPredisplay(
		eyegui::Layout* pLayout,
		std::string id);
}

#endif // EYE_GUI_H_
