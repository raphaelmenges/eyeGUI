//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef DICTIONARY_H_
#define DICTIONARY_H_

#include "src/Defines.h"

#include <string>
#include <vector>

// TODO:
// - What datastructure? for each letter a node?
// - should be able to match goofy and gofy (that is were it gets difficult)
//  - Idea: duplicate all words with the same letters in one row, delete double
//    / tripple letters and give a pointer from them to the "real word"
// - Multiple dictionaries (URLs, Language....)
// - internally std::string and functions which give back 16bit strings?

// - Somehow a Trie structure, which collects all words. Problem: words like goofy which are gazed in as gofy in fast typing mode :(

class Dictionary
{
public:

    // Constructor
    Dictionary();

    // Destructor
    virtual ~Dictionary();

    // Check for exact word
    bool checkForWord(std::u16string word) const;

    // Give similar words
    std::vector<std::u16string> similarWords(std::u16string word, uint count) const;


private:
};

#endif // DICTIONARY_H_
