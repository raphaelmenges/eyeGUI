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

// Notes:
// - Words are saved in lower case! but some enum is used to rescue information about case
// -

// TODO:
// - should be able to match goofy and gofy (that is were it gets difficult)
//  - Idea: duplicate all words with the same letters in one row, delete double
//    / tripple letters and give a pointer from them to the "real word"
// - Multiple dictionaries (URLs, Language....)

class Dictionary
{
public:

    // Constructor
    Dictionary();

    // Destructor
    virtual ~Dictionary();

    // Check for exact word
    bool checkForWord(const std::u16string& rWord) const;

    // Give similar words
    std::vector<std::u16string> similarWords(const std::u16string& rWord, uint count) const;

private:

    // Some typedef for the map
    struct Node;
    typedef std::map<char16_t,Node> NodeMap;

    // Add single word to dictionary
    void addWord(const std::u16string& rWord);

    // Possible cases for word, using 1 byte char as structure
    enum class WordCase : char {LOWER, UPPER, BOTH};

    // Inner class for node. One node per letter in each word. Buildung up a tree by reusing existing nodes
    class Node
    {
    public:

        // Constructor
        Node(char16_t letter, WordCase wordCase) : letter(letter), wordCase(wordCase) {}

        // Members
        char16_t letter; // Letter in node
        WordCase wordCase; // Case of word formed by letters in nodes from root to here
        NodeMap children; // Further possible letters to form other words
    };

    // Members
    std::map<char16_t,Node> mRootMap; // Root map

};

#endif // DICTIONARY_H_
