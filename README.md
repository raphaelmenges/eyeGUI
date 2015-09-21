# eyeGUI
This library enables one to load, manipulate and render user interface for eyetracking input defined in xml files describing layouts. To use this library, there is a function based interface in a single header which requires the usage of C++11 and OpenGL 3.3 or higher. Layouts are thought as overlay for your specific application for interaction using the gaze input from a eyetracker, however one can create a whole application using layouts. For more information about usage, please visit the [**wiki**](https://github.com/raphaelmenges/eyeGUI/wiki)!

## Version
Current version on master branch is 0.1 and tested on Linux and Windows. Nexts version will feature floating elements, text rendering and text input using the gaze. At the moment, gaze is simulated using mouse position. This will be changed, as far as i get an eyetracker for testing.

## Demo
Video hosted on YouTube, click on the preview for watching. Turn on subtitles for some explanation.
[![Screenshot](https://raw.githubusercontent.com/wiki/raphaelmenges/eyeGUI/DemoVideoLink.png)](https://youtu.be/niMRX65E7IE)

## Dependencies
* GLEW: http://glew.sourceforge.net/
* NanoSVG: https://github.com/memononen/nanosvg
* TinyXML2: https://github.com/leethomason/tinyxml2
* GLM: http://glm.g-truc.net/0.9.7/index.html
* picoPNG: http://lodev.org/lodepng/

All dependencies are already included inside the folder "External" and should be found by CMake. Keep the licenses in mind when you use this library in your project. At least, the license file of GLEW must be included.
