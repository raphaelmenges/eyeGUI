//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Dictionary.h"

#include <fstream>

// TODO: Delete
#include <iostream>

Dictionary::Dictionary()
{
    std::cout << "Hallo welt" << std::endl;

    // Read file with instream
    std::ifstream in("/home/raphael/Downloads/german.dic");

    // Just print all the words (quite a lot)
    if(in)
    {
        std::string line;
        while (getline(in, line))
        {
            std::cout << line << std::endl;
        }
    }
}

Dictionary::~Dictionary()
{
    // Nothing to do
}

bool Dictionary::checkForWord(std::u16string word) const
{
    return false;
}

std::vector<std::u16string> Dictionary::similarWords(std::u16string word, uint count) const
{
    return std::vector<std::u16string>();
}
