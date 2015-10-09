# eyeGUI
This library enables one to load, manipulate and render user interface for eyetracking input defined in xml files describing layouts. To use this library, there is a function based interface in a single header which requires the usage of C++11 and OpenGL 3.3 or higher. Layouts are thought as overlay for your specific application for interaction using the gaze input from a eyetracker, however one can create a whole application using layouts. For more information about usage, please visit the [**wiki**](https://github.com/raphaelmenges/eyeGUI/wiki)!

## Demo
Video hosted on YouTube, click on the preview for watching. Turn on subtitles for some explanations.
[![Screenshot](https://raw.githubusercontent.com/wiki/raphaelmenges/eyeGUI/DemoVideoLink.png)](https://youtu.be/niMRX65E7IE)

## Changelist
#### Version 0.2 (in development)
* Floating frames

#### Version 0.1
* Initial commit

## Dependencies
* NanoSVG: https://github.com/memononen/nanosvg
* TinyXML2: https://github.com/leethomason/tinyxml2
* GLM: http://glm.g-truc.net/0.9.7/index.html
* picoPNG: http://lodev.org/lodepng/

All dependencies are already included inside the folder "externals" and should be found by CMake. Keep the licenses in mind when you use this library in your project. But the chosen dependencies should be no problem if one wants to use just the compiled library without naming anybody.

## Acknowledgment
* Great script to create license free OpenGL loader: https://bitbucket.org/alfonse/glloadgen/wiki
