//	The MIT License (MIT)
//
//	Copyright(c) 2015 Raphael Menges
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
 *  \version   0.1
 *  \license   This project is released under the MIT License (MIT)
 */

#ifndef EYE_GUI_H_
#define EYE_GUI_H_

#include <string>
#include <memory>

namespace eyegui
{
    class GUI;
    class Layout;
    class Frame;

    //! Enumeration of possible picture alignments.
    /*! This enum is defined directly in the interface because it is needed by the replacing funtions. */
    enum class PictureAlignment { ORIGINAL, STRETCHED };

    //! Abstract listener class for buttons.
    class ButtonListener
    {
    public:

        //! Constructor.
        ButtonListener();

        //! Destructor.
        virtual ~ButtonListener();

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
        virtual ~SensorListener();

        //! Callback for penetration of sensor.
        /*!
          \param pLayout pointer to layout from which callback is coming.
          \param id is the unique id of the sensor which causes the callback.
          \param amount is the value of penetration at time of callback.
        */
        void virtual penetrated(Layout* pLayout, std::string id, float amount) = 0;

    };

    //! Creates GUI and returns pointer to it.
    /*!
      \param width of GUI as integer.
      \param height of GUI as integer.
      \return pointer to created GUI.
    */
    GUI* createGUI(int width, int height);

    //! Creates layout inside GUI and returns pointer to it.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to layout xml file.
      \param visible shall added layout be visible.
      \return pointer to added layout. Null if creation was not possible because layouts were locked.
    */
    Layout* addLayout(GUI* pGUI, std::string filepath, bool visible = true);

    //! Render whole GUI.
    /*!
      \param pGUI pointer to GUI.
      \param tpf passed time since last rendering in seconds as float.
    */
    void renderGUI(GUI* pGUI, float tpf);

    //! Terminate GUI.
    /*!
      \param pGUI pointer to GUI which should be termianted.
    */
    void terminateGUI(GUI* pGUI);

    //! Resize GUI.
    /*!
      \param pGUI pointer to GUI.
      \param width of GUI as integer.
      \param height of GUI as integer.
    */
    void resizeGUI(GUI* pGUI, int width, int height);

    //! Load config.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to config file.
    */
    void loadConfig(GUI* pGUI, std::string filepath);

    //! Set mouse cursor.
    /*!
      \param pGUI pointer to GUI.
      \param x is horizontal coordinate of mouse cursor.
      \param y is vertical coordinate of mouse cursor.
    */
    void setMouseCursor(GUI* pGUI, int x, int y);

    //! Prefetch image to avoid lags.
    /*!
      \param pGUI pointer to GUI.
      \param filepath is path to image which should be prefetched.
    */
    void prefetchImage(GUI* pGUI, std::string filepath);

    //! Control layout's input usage.
    /*!
      \param pLayout pointer to layout.
      \param useInput indicated whether layout may use input or ignore it.
    */
    void setInputUsageOfLayout(Layout* pLayout, bool useInput);

    //! Set visibility of layout.
    /*!
      \param pLayout pointer to layout.
      \param visible is a bool value to set visibility.
      \param reset indicates whether all elements in layout should be reset.
      \param setImmediately prohibits fading.
    */
    void setVisibilityOfLayout(Layout* pLayout, bool visible, bool reset = false, bool setImmediately = false);

    //! Move layout to front.
    /*!
      \param pGUI pointer to GUI.
      \param pLayout pointer to layout.
    */
    void moveLayoutToFront(GUI* pGUI, Layout* pLayout);

    //! Move layout to back.
    /*!
      \param pGUI pointer to GUI.
      \param pLayout pointer to layout.
    */
    void moveLayoutToBack(GUI* pGUI, Layout* pLayout);

    //! Activity of element.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param active indicates the state of acitvity.
      \param setImmediately prohibits fading.
    */
    void setElementActivity(Layout* pLayout, std::string id, bool active, bool setImmediately = false);

    //! Toggle activity of element.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param setImmediately prohibits fading.
    */
    void toggleElementActivity(Layout* pLayout, std::string id, bool setImmediately = false);

    //! Get activity of element.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \return true if element with given id is active and false else
    */
    bool isElementActive(Layout const * pLayout, std::string id);

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
    void highlightInteractiveElement(Layout* pLayout, std::string id, bool doHighlight);

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

    //! Set icon of interactive element.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param iconFilepath path to image which should be used as icon.
    */
    void setIconOfInteractiveElement(Layout* pLayout, std::string id, std::string iconFilepath);

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
    */
    void buttonDown(Layout* pLayout, std::string id);

    //! Button up.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
    */
    void buttonUp(Layout* pLayout, std::string id);

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

    //! Register listener to button.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param wpListener is weak pointer to listener that should be registered.
    */
    void registerButtonListener(Layout* pLayout, std::string id, std::weak_ptr<ButtonListener> wpListener);

    //! Register listener to sensor.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param wpListener is weak pointer to listener that should be registered.
    */
    void registerSensorListener(Layout* pLayout, std::string id, std::weak_ptr<SensorListener> wpListener);

    //! Replace element with block.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
    */
    void replaceElementWithBlock(Layout* pLayout, std::string id, bool doFading);

    //! Replace element with picture.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
      \param filepath is the path to the image used in the picture element.
      \param alignment is the inner alignment of the picture.
    */
    void replaceElementWithPicture(Layout* pLayout, std::string id, bool doFading, std::string filepath, PictureAlignment alignment);

    //! Replace element with blank.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
    */
    void replaceElementWithBlank(Layout* pLayout, std::string id, bool doFading);

    //! Replace element with circle button.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
      \param iconFilepath path to image which should be used as icon.
      \param isSwitch indicated, whether button should be a switch.
    */
    void replaceElementWithCircleButton(Layout* pLayout, std::string id, bool doFading, std::string iconFilepath, bool isSwitch = false);

    //! Replace element with box button.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
      \param iconFilepath path to image which should be used as icon.
      \param isSwitch indicated, whether button should be a switch.
    */
    void replaceElementWitBoxButton(Layout* pLayout, std::string id, bool doFading, std::string iconFilepath, bool isSwitch = false);

    //! Replace element with sensor.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
      \param iconFilepath path to image which should be used as icon.
    */
    void replaceElementWitSensor(Layout* pLayout, std::string id, bool doFading, std::string iconFilepath);

    //! Replace element with brick.
    /*!
      \param pLayout pointer to layout.
      \param id is the unique id of an element.
      \param dpFading indicated, whether replaced element should fade.
      \param filepath is path to brick xml file.
    */
    void replaceElementWithBrick(Layout* pLayout, std::string id, bool doFading, std::string filepath);

    //! Creates floating frame with brick inside
    /*!
      \param pLayout pointer to layout.
      \param dpFading indicated, whether frame should fade in.
      \param filepath is path to brick xml file.
      \param relativePositionX initial relative x position
      \param relativePositionY initial relative y position
      \param relativeSizeX initial relative x size
      \param relativeSizeY initial relative y size
      \param visible indicates, whether frame should be visible or not
      \return index of created floating frame
    */
    unsigned int addFloatingFrameWithBrick(
        Layout* pLayout,
        std::string filepath,
        float relativePositionX,
        float relativePositionY,
        float relativeSizeX,
        float relativeSizeY,
        bool doFading,
        bool visible = true);

    //! Set visibility of floating frame.
    /*!
      \param pLayout pointer to layout.
      \param frameIndex index of frame in layout.
      \param visible is a bool value to set visibility.
      \param reset indicates whether all elements in layout should be reset.
      \param setImmediately prohibits fading.
    */
    void setVisibilityOFloatingfFrame(Layout* pLayout, unsigned int frameIndex, bool visible, bool reset = false, bool setImmediately = false);

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
}

#endif // EYE_GUI_H_
