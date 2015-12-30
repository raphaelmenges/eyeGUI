cmake_minimum_required(VERSION 2.8.3)

# Project title
project(eyeGUILibrary)

# Title of library
set(LIBNAME "eyeGUI")

# Output path
set(LIBRARY_OUTPUT_PATH "${PROJECT_BINARY_DIR}/lib/")

# Activate C++11 in GCC and CLANG (Apple maybe needs '-stdlib=libc++')
if(NOT MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif(NOT MSVC)

# Directory of implementation
set(SRC_DIR "${CMAKE_CURRENT_LIST_DIR}/src")

# Include at top level and at source code level
include_directories("${CMAKE_CURRENT_LIST_DIR}")
include_directories("${SRC_DIR}")

# Find all sources and headers
file(GLOB_RECURSE SOURCES
	"${SRC_DIR}/*.cpp"
	"${SRC_DIR}/*.cc"
	"${SRC_DIR}/*.c")

file(GLOB_RECURSE HEADERS
	"${SRC_DIR}/*.h")

# Directory of external code
set(EXTERNAL_DIR "${CMAKE_CURRENT_LIST_DIR}/externals")

# Add TinyXML2
file(GLOB_RECURSE TINYXML2
	"${EXTERNAL_DIR}/TinyXML2/*")

# Add GLM
file(GLOB_RECURSE GLM
	"${EXTERNAL_DIR}/GLM/glm/*.hpp")
include_directories("${EXTERNAL_DIR}/GLM")

# Add OpenGLLoader
file(GLOB_RECURSE OPEN_GL_LOADER
	"${EXTERNAL_DIR}/OpenGLLoader/*")

# Add stb_image
file(GLOB_RECURSE STB_IMAGE
	"${EXTERNAL_DIR}/stb/*")

# Add NanoSVG
file(GLOB_RECURSE NANO_SVG
	"${EXTERNAL_DIR}/NanoSVG/*.h")

# Add FreeType2 (list of source code taken from ./externals/FreeType2/CMakeLists.txt)
add_definitions(-DFT2_BUILD_LIBRARY)
set(FREETYPE2_DIR "${EXTERNAL_DIR}/FreeType2")
file(GLOB FREETYPE2
	"${FREETYPE2_DIR}/include/ft2build.h"
	"${FREETYPE2_DIR}/include/freetype/*.h"
	"${FREETYPE2_DIR}/include/freetype/config/*.h"
	"${FREETYPE2_DIR}/include/freetype/internal/*.h"
	"${FREETYPE2_DIR}/src/autofit/autofit.c"
	"${FREETYPE2_DIR}/src/base/ftbase.c"
	"${FREETYPE2_DIR}/src/base/ftbbox.c"
	"${FREETYPE2_DIR}/src/base/ftbdf.c"
	"${FREETYPE2_DIR}/src/base/ftbitmap.c"
	"${FREETYPE2_DIR}/src/base/ftcid.c"
	"${FREETYPE2_DIR}/src/base/ftfntfmt.c"
	"${FREETYPE2_DIR}/src/base/ftfstype.c"
	"${FREETYPE2_DIR}/src/base/ftgasp.c"
	"${FREETYPE2_DIR}/src/base/ftglyph.c"
	"${FREETYPE2_DIR}/src/base/ftgxval.c"
	"${FREETYPE2_DIR}/src/base/ftinit.c"
	"${FREETYPE2_DIR}/src/base/ftlcdfil.c"
	"${FREETYPE2_DIR}/src/base/ftmm.c"
	"${FREETYPE2_DIR}/src/base/ftotval.c"
	"${FREETYPE2_DIR}/src/base/ftpatent.c"
	"${FREETYPE2_DIR}/src/base/ftpfr.c"
	"${FREETYPE2_DIR}/src/base/ftstroke.c"
	"${FREETYPE2_DIR}/src/base/ftsynth.c"
	"${FREETYPE2_DIR}/src/base/ftsystem.c"
	"${FREETYPE2_DIR}/src/base/fttype1.c"
	"${FREETYPE2_DIR}/src/base/ftwinfnt.c"
	"${FREETYPE2_DIR}/src/bdf/bdf.c"
	"${FREETYPE2_DIR}/src/bzip2/ftbzip2.c"
	"${FREETYPE2_DIR}/src/cache/ftcache.c"
	"${FREETYPE2_DIR}/src/cff/cff.c"
	"${FREETYPE2_DIR}/src/cid/type1cid.c"
	"${FREETYPE2_DIR}/src/gzip/ftgzip.c"
	"${FREETYPE2_DIR}/src/lzw/ftlzw.c"
	"${FREETYPE2_DIR}/src/pcf/pcf.c"
	"${FREETYPE2_DIR}/src/pfr/pfr.c"
	"${FREETYPE2_DIR}/src/psaux/psaux.c"
	"${FREETYPE2_DIR}/src/pshinter/pshinter.c"
	"${FREETYPE2_DIR}/src/psnames/psnames.c"
	"${FREETYPE2_DIR}/src/raster/raster.c"
	"${FREETYPE2_DIR}/src/sfnt/sfnt.c"
	"${FREETYPE2_DIR}/src/smooth/smooth.c"
	"${FREETYPE2_DIR}/src/truetype/truetype.c"
	"${FREETYPE2_DIR}/src/type1/type1.c"
	"${FREETYPE2_DIR}/src/type42/type42.c"
	"${FREETYPE2_DIR}/src/winfonts/winfnt.c"
)

if(WIN32)
	set(FREETYPE2 ${FREETYPE2} "${FREETYPE2_DIR}/builds/windows/ftdebug.c")
else(WIN32)
	set(FREETYPE2 ${FREETYPE2} "${FREETYPE2_DIR}/src/base/ftdebug.c")
endif(WIN32)

# Including for FreeType
include_directories("${FREETYPE2_DIR}/include")

# Add UTF8-CPP
set(UFTCPP_DIR "${EXTERNAL_DIR}/utfcpp/source")
file(GLOB_RECURSE UTFCPP
	"${UFTCPP_DIR}/*.h")
include_directories("${UFTCPP_DIR}")

# Collect all code
set(ALL_CODE
	${SOURCES}
	${HEADERS}
	${TINYXML2}
	${GLM}
	${OPEN_GL_LOADER}
	${STB_IMAGE}
	${NANO_SVG}
	${FREETYPE2}
	${UTFCPP})

# Filtering for Visual Studio
IF(MSVC)

	# http://stackoverflow.com/questions/9701387/cmake-source-group-multiple-files
	foreach(f ${ALL_CODE})
		# Get the path of the file relative to ${CMAKE_CURRENT_SOURCE_DIR},
		# then alter it (not compulsory)
		file(RELATIVE_PATH SRCGR "${CMAKE_CURRENT_LIST_DIR}" ${f})
		set(SRCGR "${LIBNAME}/${SRCGR}")

		# Extract the folder, ie remove the filename part
		string(REGEX REPLACE "(.*)(/[^/]*)$" "\\1" SRCGR ${SRCGR})

		# Source_group expects \\ (double antislash), not / (slash)
		string(REPLACE / \\ SRCGR ${SRCGR})
		source_group("${SRCGR}" FILES ${f})
	endforeach()

ENDIF(MSVC)

# Add the interface header file
set(ALL_CODE ${ALL_CODE} "${CMAKE_CURRENT_LIST_DIR}/include/eyeGUI.h")

# Create eyeGUI static library
add_library(${LIBNAME} STATIC ${ALL_CODE})