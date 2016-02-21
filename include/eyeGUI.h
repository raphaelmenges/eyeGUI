//	The MIT License (MIT)
//
//	Copyright(c) 2016 Raphael Menges
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
 *  \version   0.8
 *  \license   This project is released under the MIT License (MIT)
 */

#ifndef EYE_GUI_H_
#define EYE_GUI_H_

#include <string>
#include <memory>
#include <map>

namespace eyegui
{
    class GUI;
    class Layout;
    class Frame;

    //! Enumeration of possible character sets for font rendering.
    /*! This enum is defined directly in the interface because it is needed by initialization. */
    enum class CharacterSet { GERMANY_GERMAN, US_ENGLISH };

    //! Enumeration of possible font sizes.
    /*! This enum is defined directly in the interface because it is needed by some functions. */
    enum class FontSize { TALL, MEDIUM, SMALL };

    //! Enumeration of possible text flow alignments
    /*! This enum is defined directly in the interface because it is needed by some functions. */
    enum class TextFlowAlignment { LEFT, RIGHT, CENTER, JUSTIFY };

    //! Enumeration of possible vertical text flow alignments
    /*! This enum is defined directly in the interface because it is needed by some functions. */
    enum class TextFlowVerticalAlignment { TOP, CENTER, BOTTOM };

    //! Enumeration of possible image alignments.
    /*! This enum is defined directly in the interface because it is needed by the replacing funtions. */
    enum class ImageAlignment { ORIGINAL, STRETCHED, ZOOMED };

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

    //! Struct for relative values of position and size
    struct RelativePositionAndSize
    {
        float x = 0;
        float y = 0;
        float width = 0;
        float height = 0;
    };

    //! Struct for absolute pixel values of position and size
    struct AbsolutePositionAndSize
    {
        int x = 0;
        int y = 0;
        int width = 0;
        int height = 0;
    };

    //! Struct for input
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

        GUI* construct() const; //!< returns pointer to built GUI
        int width; //!< width of GUI as integer
        int height; //!< height of GUI as integer
        std::string fontFilepath = ""; //!< fontFilepath is path to a .ttf font file
        CharacterSet characterSet = CharacterSet::US_ENGLISH; //!< characterSet used to initialize font rendering
        std::string localizationFilepath = ""; //!< localizationFilepath is path to a .leyegui file
        float vectorGraphicsDPI = 96.0f; //!< dpi which are used to rasterize vector graphics
        float fontTallSize = 0.1f; //!< Height of tall font in percentage of GUI height
        float fontMediumSize = 0.04f; //!< Height of medium font in percentage of GUI height
        float fontSmallSize = 0.0175f; //!< Height of small font in percentage of GUI height
    };

    //! Creates layout inside GUI and returns pointer to it. Is executed at update call.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to layout xml file.
      \param visible shall added layout be visible.
      \return pointer to added layout. Null if creation was not possible because layouts were locked.
    */
    Layout* addLayout(GUI* pGUI, std::string filepath, bool visible = true);

    //! Removes layout in GUI by pointer. Is executed at update call.
    /*!
      \param pGUI pointer to GUI.
      \param pLayout is the pointer to layout which should be removed.
    */
    void removeLayout(GUI* pGUI, Layout const * pLayout);

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

    //! Load config. Is executed at update call.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to config file.
    */
    void loadConfig(GUI* pGUI, std::string filepath);

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

    //! Prefetch image to avoid lags.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to image which should be prefetched.
    */
    void prefetchImage(GUI* pGUI, std::string filepath);

    //! Sets value of config attribute. Is executed at update call.
    /*!
    \param pLayout pointer to layout.
    \param attribute is name of attribute which shall be changed.
    \param value is new value of attribute.
    */
    void setValueOfConfigAttribute(
        GUI* pGUI,
        std::string attribute,
        std::string value);

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

    //! Control layout's input usage.
    /*!
      \param pLayout pointer to layout.
      \param useInput indicates whether layout may use input or ignore it.
    */
    void setInputUsageOfLayout(Layout* pLayout, bool useInput);

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

    //! Set style of element.
    /*!
    \param pLayout pointer to layout.
    \param id is the unique id of an element.
    \param stlye is name of style in stylesheet of layout.
    */
    void setStyleOfElement(
        Layout* pLayout,
        std::string id,
        std::string style);

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

    //! Sets value of style attribute.
    /*!
      \param pLayout pointer to layout.
      \param styleName is name of style in used stylesheet.
      \param attribute is name of attribute which shall be changed.
      \param r is red color value.
      \param g is green color value.
      \param b is blue color value.
      \param a is alpha color value.
    */
    void setValueOfStyleAttribute(
        Layout* pLayout,
        std::string styleName,
        std::string attribute,
        float r,
        float g,
        float b,
        float a);

    //! Set icon of icon interactive element.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param iconFilepath path to image which should be used as icon.
    */
    void setIconOfIconInteractiveElement(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath);

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
    \param content is new content for text block as 16 bit string.
    */
    void setContentOfTextBlock(Layout* pLayout, std::string id, std::u16string content);

    //! Set content of text block. Works only if no key is used for localization.
    /*!
    \param pLayout pointer to layout.
    \param id is the unique id of an element.
    \param content is new content for text block as 8 bit string.
    */
    void setContentOfTextBlock(Layout* pLayout, std::string id, std::string content);

    //! Set key of text block. Works only if used localization file includes key.
    /*!
    \param pLayout pointer to layout.
    \param id is the unique id of an element.
    \param key is new key for text block.
    */
    void setKeyOfTextBlock(Layout* pLayout, std::string id, std::string key);

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

    //! Replace element with block.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param consumeInput indicates, whether block consumes given input.
      \param backgroundFilepath is path to image rendered in background.
             Use empty string to indicate no background image.
      \param backgroundAlignment indicates alignment of background image.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithBlock(
        Layout* pLayout,
        std::string id,
        bool consumeInput,
        std::string backgroundFilepath = "",
        ImageAlignment backgroundAlignment = ImageAlignment::ZOOMED,
        bool fade = false);

    //! Replace element with picture.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param filepath is the path to the image used in the picture element.
      \param alignment is the alignment of the picture.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithPicture(
        Layout* pLayout,
        std::string id,
        std::string filepath,
        ImageAlignment alignment,
        bool fade = false);

    //! Replace element with blank.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithBlank(
        Layout* pLayout,
        std::string id,
        bool fade = false);

    //! Replace element with circle button.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param iconFilepath path to image which should be used as icon.
      \param isSwitch indicates, whether button should be a switch.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithCircleButton(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath,
        bool isSwitch = false,
        bool fade = false);

    //! Replace element with box button.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param iconFilepath path to image which should be used as icon.
      \param isSwitch indicates, whether button should be a switch.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithBoxButton(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath,
        bool isSwitch = false,
        bool fade = false);

    //! Replace element with sensor.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param iconFilepath path to image which should be used as icon.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithSensor(
        Layout* pLayout,
        std::string id,
        std::string iconFilepath,
        bool fade = false);

    //! Replace element with text block.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param consumeInput indicates, whether block consumes given input.
      \param fontSize is size of used font.
      \param alignment is alignment of text.
      \param verticalAlignment is vertical alignment of text.
      \param content is the content of the displayed text.
	  \param innerBorder is space between border and text.
      \param textScale is scale of text.
      \param key is used for localization.
	  \param backgroundFilepath is path to image rendered in background.
			 Use empty string to indicate no background image.
	  \param backgroundAlignment indicates alignment of background image.
      \param fade indicates, whether replaced element should fade.
    */
    void replaceElementWithTextBlock(
        Layout* pLayout,
        std::string id,
        bool consumeInput,
        FontSize fontSize,
        TextFlowAlignment alignment,
        TextFlowVerticalAlignment verticalAlignment,
        std::u16string content,
        float innerBorder = 0.0f,
		float textScale = 1.0f,
        std::string key = "",
        std::string backgroundFilepath = "",
        ImageAlignment backgroundAlignment = ImageAlignment::ZOOMED,
        bool fade = false);

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
    void setVisibilityOFloatingfFrame(
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
      \param pCallbackFunction is function pointer to function which should be called back.
    */
    void setErrorCallback(void(*pCallbackFunction)(std::string));

    //! Set warning callback function.
    /*!
      \param pCallbackFunction is function pointer to function which should be called back.
    */
    void setWarningCallback(void(*pCallbackFunction)(std::string));

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

#endif // EYE_GUI_H_
