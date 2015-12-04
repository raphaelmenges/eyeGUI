//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Helps to build full path out of relative filepath and root filepath. Root
// filepath may only be set by eyeGUI.cpp function "setRootFilepath". Everything
// else would be dangerous.

#ifndef PATHER_H_
#define PATHER_H_

#include <string>

namespace eyegui
{
    class Pather
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
        static Pather* getInstance()
        {
            if (pInstance == NULL)
            {
                static Pather pather;
                pInstance = &pather;
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
        static Pather* pInstance;

        // Filepath
        static std::string rootFilepath;
    };
}

#endif // PATHER_H_
