# eyeGUI
This library enables one to load, manipulate and render user interfaces for eyetracking input defined in xml files describing layouts. To use this library, there is a function based interface in a single header which requires the usage of C++11 and OpenGL 3.3 or higher. Layouts are thought as overlay for your specific application for interaction with the gaze input from an eyetracker, however one can create a whole application using layouts. For more information about usage, please visit the [**wiki**](https://github.com/raphaelmenges/eyeGUI/wiki)!

## Demo
Video hosted on YouTube, click on the preview for watching. Turn on subtitles for some explanations.
[![Screenshot](https://raw.githubusercontent.com/wiki/raphaelmenges/eyeGUI/DemoVideoLink.png)](https://youtu.be/niMRX65E7IE)

## Features

#### Version 0.8
* Simple keyboard
* Image background for blocks possible
* Text can be scaled

#### Version 0.7
* Bugfix release

#### Version 0.6
* stb_image for pixel image loading
* Marking of elements
* Instant interaction with element beneath gaze (thanks for the idea to Wojciech Kwasnik)

#### Version 0.5
* Extending interface
* Layout can be removed from GUI
* Hiding of elements
* Root of all filepaths can be set

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

## HowTo
In order to use eyeGUI, please follow these steps:
* Generate a project for your compiler with CMake (somewhere on your computer)
* Compile that project (tested with GCC 5.x, Visual Studio 2015 and XCode)
* Inside your build folder chosen in CMake, there should be a folder called _lib_ containing the static library
* Just include the header from the _include_ folder into your project and link your executeable against the library
* Keep the licenses in mind! (at least, FreeType2 must be mentioned)

## Dependencies
* NanoSVG: https://github.com/memononen/nanosvg
* TinyXML2: https://github.com/leethomason/tinyxml2
* GLM: http://glm.g-truc.net/0.9.7/index.html (MIT license chosen)
* stb_image: https://github.com/nothings/stb
* FreeType 2.6.1: http://www.freetype.org/ (FreeType license chosen)
* UTF8-CPP: https://github.com/nemtrif/utfcpp

All dependencies are already included inside the folder _externals_ and should be found by CMake. They were updated at 23rd October 2015 in master branch. Keep the licenses in mind when you use this library in your project. The source code of this project itself in the folder _src_ and the header _eyeGUI.h_ from _include_ is licensed under the MIT license. From my point, it seems you only have to advertize the FreeType Library by mentioning it in your documentation or program when using the compiled library. Errors and omissions excepted ;)

## Acknowledgments
* Script to create license free OpenGL loader: https://bitbucket.org/alfonse/glloadgen/wiki

## License
>The MIT License (MIT)

>Copyright (c) 2016 Raphael Menges

>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

>The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

>THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
