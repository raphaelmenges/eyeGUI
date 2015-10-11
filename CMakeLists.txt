cmake_minimum_required(VERSION 2.8)

# Project title
project(eyeGUILibrary)

# Title of library
set(LIBNAME eyeGUI)

# Output path
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build/lib/)

# Activate C++11 in GCC and CLANG
IF(NOT MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -stdlib=libc++")
ENDIF(NOT MSVC)

# Directory of source code
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Include at top level and at source code level
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${SRC_DIR})

# Find all sources and headers
file(GLOB_RECURSE SOURCES
    "${SRC_DIR}/*.cpp"
    "${SRC_DIR}/*.cc"
    "${SRC_DIR}/*.c")

file(GLOB_RECURSE HEADERS
    "${SRC_DIR}/*.h")

# Directory of external stuff
set(EXTERNAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/externals)

# Add TinyXML2
file(GLOB_RECURSE TINYXML2
    "${EXTERNAL_DIR}/TinyXML2/*")

# Add GLM
file(GLOB_RECURSE GLM
    "${EXTERNAL_DIR}/GLM/glm/*.hpp")
include_directories(${EXTERNAL_DIR}/GLM)

# Add OpenGLLoader
file(GLOB_RECURSE OpenGLLoader
    "${EXTERNAL_DIR}/OpenGLLoader/*")

# Add picoPNG
file(GLOB_RECURSE PICO_PNG
    "${EXTERNAL_DIR}/picoPNG/*")

# Add NanoSVG
file(GLOB_RECURSE NANO_SVG
    "${EXTERNAL_DIR}/NanoSVG/*.h")

# Collect all code
set(ALL_CODE
    ${SOURCES}
    ${HEADERS}
    ${TINYXML2}
    ${GLM}
    ${OpenGLLoader}
    ${PICO_PNG}
    ${NANO_SVG})

# Filtering for Visual Studio
IF(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${ALL_CODE})
		# Get the path of the file relative to ${CMAKE_CURRENT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_SOURCE_DIR}" ${f})
		set(SRCGR "${APPNAME}/${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

ENDIF(MSVC)

# Add the interface header file
set(ALL_CODE ${ALL_CODE} eyeGUI.h)

# Create library
add_library(${LIBNAME} STATIC ${ALL_CODE})

# Copy header to build directory
add_custom_command(TARGET ${LIBNAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_SOURCE_DIR}/eyeGUI.h ${CMAKE_BINARY_DIR}/build/include/eyeGUI.h)
