# eyeGUI
This library enables one to load, manipulate and render user interfaces for eyetracking input defined in xml files describing layouts. To use this library, there is a function based interface in a single header which requires the usage of C++11 and OpenGL 3.3 or higher. Layouts are thought as overlay for your specific application for interaction with the gaze input from an eyetracker, however one can create a whole application using layouts. For more information about usage, please visit the [**wiki**](https://github.com/raphaelmenges/eyeGUI/wiki)!

## Demo
Video hosted on YouTube, click on the preview for watching. Turn on subtitles for some explanations.
[![Screenshot](https://raw.githubusercontent.com/wiki/raphaelmenges/eyeGUI/DemoVideoLink.png)](https://youtu.be/niMRX65E7IE)

## Features

#### Version 0.5
* Extending interface
* Layout can be removed from GUI
* Hiding of elements
* Root of all filepathes can be set

#### Version 0.4
* Gaze visualization
* Input improvement

#### Version 0.3
* Adaptive scaling
* Id replacement at brick loading

#### Version 0.2
* Dimming
* Localization
* Font rendering
* Floating frames

#### Version 0.1
* Initial commit

## Dependencies
* NanoSVG: https://github.com/memononen/nanosvg
* TinyXML2: https://github.com/leethomason/tinyxml2
* GLM: http://glm.g-truc.net/0.9.7/index.html (MIT license chosen)
* picoPNG: http://lodev.org/lodepng/
* FreeType 2.6.1: http://www.freetype.org/ (FreeType license chosen)
* UTF8-CPP: https://github.com/nemtrif/utfcpp

All dependencies are already included inside the folder "externals" and should be found by CMake. They were updated at 23rd October 2015 in master branch. Keep the licenses in mind when you use this library in your project. The source code of this project itself in the folder "src" and the header "eyeGUI.h" is licensed under the MIT license. From my point, it seems you only have to "advertize" the FreeType Library by mentioning it in your documentation or program when using the compiled library. Errors and omissions excepted ;)

## Acknowledgments
* Script to create license free OpenGL loader: https://bitbucket.org/alfonse/glloadgen/wiki
