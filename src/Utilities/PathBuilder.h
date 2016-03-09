//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Helps to build full path out of relative filepath and root filepath. Root
// filepath may only be set by eyeGUI.cpp function "setRootFilepath". Everything
// else would be dangerous.

#ifndef PATH_BUILDER_H_
#define PATH_BUILDER_H_

#include <string>

// Defines for easier use
#define buildPath eyegui::PathBuilder::buildFullFilepath

namespace eyegui
{
    class PathBuilder
    {
    public:

        // Only friend may set root filepath
        friend void setRootFilepath(std::string rootFilepath);

        // Build full filepath
        static std::string buildFullFilepath(std::string filepath)
        {
            return getInstance()->internalBuildFullFilepath(filepath);
        }

    private:

        // Check for instance existence
        static PathBuilder* getInstance()
        {
            if (pInstance == NULL)
            {
                static PathBuilder pathBuilder;
                pInstance = &pathBuilder;
            }
            return pInstance;
        }

        // Internal function to build full filepath
        std::string internalBuildFullFilepath(std::string filepath)
        {
            std::string prefix = "";
            if(rootFilepath != "")
            {
                // May access root filepath
                prefix = rootFilepath + "/";
            }
            return prefix + filepath;
        }

        // The pointer to the single instance
        static PathBuilder* pInstance;

        // Filepath
        static std::string rootFilepath;
    };
}

#endif // PATH_BUILDER_H_
