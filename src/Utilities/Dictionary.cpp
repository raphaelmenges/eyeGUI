//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Dictionary.h"

#include "Helper.h"

#include <fstream>

// TODO: Delete
#include <iostream>

namespace eyegui
{
    Dictionary::Dictionary()
    {
        std::cout << "Start filling dictionary!" << std::endl;

        // Read file with instream
        //std::ifstream in("C:/Users/Raphael/Desktop/ger.txt");
        std::ifstream in("/home/raphael/Desktop/ger.txt");

        // Build up dictionary (TODO: CR / LF stuff)
        if(in)
        {
            std::string line;
            while (getline(in, line))
            {
                // Convert to utf-16 string
                std::u16string line16;
                if (!(line.empty()) && convertUTF8ToUTF16(line, line16))
                {
                    // Add word to dictionary
                    addWord(line16);
                }
            }
        }

        // TODO: TESTING AREA

        // Testing whether words can be found
        std::cout << "Start dictionary tests!" << std::endl;

        // Exact check for word
        std::cout << "CheckForWord 1: " << checkForWord(u"\u00c4rger") << std::endl; // Aerger
        std::cout << "CheckForWord 2: " << checkForWord(u"\u00e4rger") << std::endl; // aerger

        // Fuzzy word search
        auto testA = similarWords(u"Wissen", 5);
        std::cout << "Count of similar words 1: " << testA.size() << std::endl;
        for (const auto& word : testA)
        {
            std::string out;
            convertUTF16ToUTF8(word, out);
            std::cout << out << std::endl;
        }

        std::cout << "Dictionary finished!" << std::endl;
    }

    Dictionary::~Dictionary()
    {
        // Nothing to do
    }

    bool Dictionary::checkForWord(const std::u16string& rWord) const
    {
        // Convert to lower case and remember it
        std::u16string lowerWord = rWord;
        WordState wordState = convertToLower(lowerWord);

        // Pointer to map
        NodeMap const * pMap = &mRootMap;

        // Pointer to node
        Node const * pNode = NULL;

        // Go over nodes
        uint count = (uint)lowerWord.size();
        for (uint i = 0; i < count; i++)
        {
            // Try to find letter in current map
            const char16_t c = lowerWord[i];
            NodeMap::const_iterator it = pMap->find(c);
            if (it == pMap->end())
            {
                return false;
            }
            else
            {
                if (!(pMap->empty()))
                {
                    pNode = &(it->second);
                    pMap = &(pNode->children);
                }
            }
        }

        // Check whether found word has correct case
        if (pNode->wordState == wordState || pNode->wordState == WordState::BOTH_STARTS)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::vector<std::u16string> Dictionary::similarWords(const std::u16string& rWord, uint maxCount) const
    {
        // Convert to lower case
        std::u16string lowerWord = rWord;
        WordState wordState = convertToLower(lowerWord);

        // Create set to collect results
        std::set<std::u16string> resultSet;

        // Depth of recursion
        const uint RECURSION_DEPTH = 3;

        // Search for given word, ignore identical letters in one row
        fuzzyWordSearch(lowerWord, RECURSION_DEPTH, resultSet);

        // Copy results from set to vector
        std::vector<std::u16string> resultVector;
        std::copy(resultSet.begin(), resultSet.end(), std::back_inserter(resultVector));

        // TODO:
        // - Sort words after similiarity to given word (remind case)
        // - Give back max count of words (see parameters)

        // Return what you have (could be nothing)
        return resultVector;
    }

    void Dictionary::addWord(const std::u16string& rWord)
    {
        // Convert to lower case and remember it
        std::u16string lowerWord = rWord;
        WordState wordState = convertToLower(lowerWord);

        // Pointer to map
        NodeMap* pMap = &mRootMap;

        // Pointer to node
        Node* pNode = NULL;

        // Go over characters in word
        uint count = (uint)lowerWord.size();
        NodeMap::iterator it;
        for (uint i = 0; i < count; i++)
        {
            // Try to find letter in current map
            const char16_t c = lowerWord[i];
            it = pMap->find(c);
            if (it == pMap->end())
            {
                // Add it to map and go on
                pMap->insert(std::make_pair(c, Node(c)));
            }

            // Remember current node and new map
            pNode = &(pMap->at(c));
            pMap = &(pNode->children);
        }

        // Set word case in last letter
        // NONE is initial value
        if (pNode->wordState == WordState::NONE)
        {
            // First occurence, use given case
            pNode->wordState = wordState;
        }
        else if (pNode->wordState != wordState)
        {
            // Already saved with other case
            pNode->wordState = WordState::BOTH_STARTS;
        }
        // else: tried to add already existing word
    }

    Dictionary::WordState Dictionary::convertToLower(std::u16string& rWord) const
    {
        WordState wordState = WordState::LOWER_START;
        std::u16string lowerWord = rWord;
        toLower(lowerWord); // One could use return value for error checking
        if (lowerWord[0] != rWord[0])
        {
            // Something has changed
            wordState = WordState::UPPER_START;
        }
        rWord = lowerWord;
        return wordState;
    }

    void Dictionary::fuzzyWordSearch(
        const std::u16string& rLowerWord,
        uint recursionDepth,
        std::set<std::u16string>& rFoundWords) const
    {
        // Just pipe it (start the recursion)
        fuzzyWordSearch(rLowerWord, 0, u"", NULL, (int)recursionDepth, rFoundWords);
    }

    void Dictionary::fuzzyWordSearch(
        const std::u16string& rLowerWord,
        uint wordStartIndex,
        std::u16string collectedWord,
        Node const * pNode,
        int remainingRecursions,
        std::set<std::u16string>& rFoundWords) const
    {
        // Check whether recursion depth is reached
        if(remainingRecursions < 0)
        {
            // Yes, skip this call
            return;
        }
        else
        {
            // No, decrement remaining recursions
            remainingRecursions--;
        }

        // Pointer to root map
        NodeMap const * pMap = &mRootMap;

        // Get pointer to map provided by node
        if (pNode != NULL)
        {
            // Use the the map given in node
            pMap = &(pNode->children);
        }

        // Consume letters from start index
        uint count = (uint)rLowerWord.size();
        for (uint i = wordStartIndex; i < count; i++)
        {
            // Current letter
            const char16_t& c = rLowerWord[i];

            // Check for similar words with one repeating letter less
            if (i > 0)
            {
                // "Aal" -> should be found typing "Aaal" (too many letters)
                if (c == rLowerWord[i-1])
                {
                    fuzzyWordSearch(rLowerWord, i + 1, collectedWord, pNode, remainingRecursions, rFoundWords);
                }
            }

            // Try to find letter in current map
            NodeMap::const_iterator it = pMap->find(c);
            if (it == pMap->end())
            {
                // No word found and nothing added
                return;
            }
            else
            {
                // Some node was found, so remember that
                pNode = &(it->second);

                // Add letter to collected word
                collectedWord += c;

                // "Aal" -> should be found typing "Al" (not enough letters, repeating ones missing)

                fuzzyWordSearch(rLowerWord, i, collectedWord, pNode, remainingRecursions, rFoundWords);

                // Only add word when no letters in input are left
                if (i + 1  == count)
                {
                    // Adds word to found words
                    addFuzzyWord(collectedWord, pNode->wordState, rFoundWords);
                }
                else if (!(pMap->empty()))
                {
                    pMap = &(pNode->children);
                }
                else
                {
                    // No nodes available, break
                    break;
                }
            }
        }

        // Input word is empty. Now one could add words which have this word as prefix
        if(pNode != NULL)
        {
            // TODO: Collect further words
        }
    }

    void Dictionary::addFuzzyWord(const std::u16string rCollectedWord, WordState collectedState, std::set<std::u16string>& rFoundWords) const
    {
        // Decide how to add word
        if (collectedState == WordState::BOTH_STARTS)
        {
            // Make version with upper case
            std::u16string collectedWordUpper = rCollectedWord;
            firstCharacterToUpper(collectedWordUpper);

            // Add both cases
            rFoundWords.insert(collectedWordUpper);
            rFoundWords.insert(rCollectedWord);
        }
        else if (collectedState == WordState::UPPER_START)
        {
            // Add upper case word
            std::u16string collectedWordUpper = rCollectedWord;
            firstCharacterToUpper(collectedWordUpper);
            rFoundWords.insert(collectedWordUpper);
        }
        else
        {
            // Could also have state NONE, but should not
            rFoundWords.insert(rCollectedWord);
        }
    }
}
