//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Helps to build full path out of relative filepath and root filepath. Root
// filepath may only be set by eyeGUI.cpp function "setRootFilepath". Everything
// else would be dangerous.

#ifndef PATH_FINDER_H_
#define PATH_FINDER_H_

#include <string>

namespace eyegui
{
    class PathFinder
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
        static PathFinder* getInstance()
        {
            if (pInstance == NULL)
            {
                static PathFinder pathFinder;
                pInstance = &pathFinder;
            }
            return pInstance;
        }

        // Internal function to build full filepath
        std::string internalBuildFullFilepath(std::string filepath)
        {
            // May access root filepath
            return rootFilepath + filepath;
        }

        // The pointer to the single instance
        static PathFinder* pInstance;

        // Filepath
        static std::string rootFilepath;
    };
}

#endif // PATHER_H_
