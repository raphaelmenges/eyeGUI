//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Dictionary.h"

#include "externals/utfcpp/source/utf8.h"

#include <fstream>

// TODO: Delete
#include <iostream>

Dictionary::Dictionary()
{
    std::cout << "Start filling dictionary!" << std::endl;

    // Read file with instream
    std::ifstream in("/home/raphael/Downloads/german.dic");

    // Build up dictionary (TODO: CR / LF stuff)
    if(in)
    {
        std::string line;
        while (getline(in, line))
        {
            // Convert to utf-16 string
            std::u16string line16;
            utf8::utf8to16(line.begin(), line.end(), back_inserter(line16));

            // Add word to dictionary
            addWord(line16);
        }
    }

    // Testing whether words can be found
    std::cout << "Start dictionary tests!" << std::endl;

    std::cout << checkForWord(u"Haus") << std::endl;
    std::cout << checkForWord(u"Huuus") << std::endl;

    std::cout << "Dictionary finished!" << std::endl;
}

Dictionary::~Dictionary()
{
    // Nothing to do
}

bool Dictionary::checkForWord(const std::u16string& rWord) const
{
    // TODO: just a rough test

    // Pointer to root map
    NodeMap const * pMap = &mRootMap;

    // Go over nodes
    uint count = rWord.size();
    for(uint i = 0; i < count; i++)
    {
        // Try to find letter in current map
        const char16_t c = rWord[i];
        NodeMap::const_iterator it = pMap->find(c);
        if(it == pMap->end())
        {
            return false;
        }
        else
        {
            if(!(pMap->empty()))
            {
                pMap = &(it->second.children);
            }
        }
    }
    return true;
}

std::vector<std::u16string> Dictionary::similarWords(const std::u16string& rWord, uint count) const
{
    // TODO
    return std::vector<std::u16string>();
}

void Dictionary::addWord(const std::u16string& rWord)
{
    // TODO: make lower case and remember what you have done
    // TODO: if same word with other case exists, change case to BOTH

    // Pointer to root map
    NodeMap* pMap = &mRootMap;

    // Go over characters in word
    uint count = rWord.size();
    for(uint i = 0; i < count; i++)
    {
        // Try to find letter in current map
        const char16_t c = rWord[i];
        NodeMap::iterator it = pMap->find(c);
        if(it == pMap->end())
        {
            // Add it to map and go on
            pMap->insert(std::make_pair(c,Node(c, WordCase::UPPER)));
        }
        // Remember new map
        pMap = &(pMap->at(c).children);
    }
}
