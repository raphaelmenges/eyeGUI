//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Dictionary.h"

#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
#include "externals/levenshtein-sse/levenshtein-sse.hpp"

#include <fstream>
#include <algorithm>

// TODO: Delete
#include <iostream>

namespace eyegui
{
    Dictionary::Dictionary()
    {
        std::cout << "Start filling dictionary!" << std::endl;

        std::string filepath = "/home/raphael/Desktop/ger.txt"; // Testing
        // std::string filepath = "/home/raphael/Temp/german.dic"; // Testing

        // Read file with instream
        std::ifstream in(filepath);

        // Build up dictionary (TODO: CR / LF stuff)
        if(in)
        {
            std::string line;
            while (getline(in, line))
            {
                // Convert to utf-16 string
                if (!(line.empty()))
                {
                    std::u16string line16;
                    if(convertUTF8ToUTF16(line, line16))
                    {
                        // Add word to dictionary
                        addWord(line16);
                    }
                    else
                    {
                        throwError(OperationNotifier::Operation::DICTIONARY, "Following line could not be parsed: " + line, filepath);
                    }
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
        std::u16string testString = u"denk";
        auto testA = similarWords(testString);
        std::cout << "Count of similar words: " << testA.size() << std::endl;
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

        // Pointer to current node
        Node const * pNode = NULL;

        // Pointer to current map
        NodeMap const * pMap = &mRootMap;

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
                    pMap = &(pNode->children); // Map of current pointer
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

    std::vector<std::u16string> Dictionary::similarWords(const std::u16string& rWord) const
    {
        // Convert to lower case
        std::u16string lowerWord = rWord;
        convertToLower(lowerWord); // state is not necessary to remember

        // Create set to collect results
        std::set<std::u16string> resultSet;

        // Depth of recursion for similar words
        const uint RECURSION_DEPTH = 3;
        const uint INPUT_PAUSE_DEPTH = 1;
        const uint INPUT_IGNORE_DEPTH = 1;

        // Search for given word, ignore identical letters in one row
        fuzzyWordSearch(
            lowerWord,
            RECURSION_DEPTH,
            INPUT_PAUSE_DEPTH,
            INPUT_IGNORE_DEPTH,
            resultSet);

        // Save size of result set
        int sizeOfResultSet = resultSet.size();

        // Only continue when necessary
        std::vector<std::u16string> resultVector;
        if(sizeOfResultSet > 0)
        {

            // Rate results by word distance
            int distance;
            std::vector<std::pair<std::u16string, int> > sortedResults;
            sortedResults.reserve(sizeOfResultSet);
            for(const std::u16string& rFoundWord : resultSet)
            {
                // Use unchanged input word to make use of case
                distance = (int)levenshteinSSE::levenshtein(rWord, rFoundWord);

                // Build up structure with found word and distance to searched one
                sortedResults.push_back(std::make_pair(rFoundWord, distance));
            }

            // Sort the built structure
            std::sort(
                sortedResults.begin(),
                sortedResults.end(),
                [](const std::pair<std::u16string, int>& left, const std::pair<std::u16string, int>& right)
                {
                    return left.second < right.second;
                });

            // Copy results to final vector
            resultVector.reserve(sizeOfResultSet);
            std::transform(
                sortedResults.begin(),
                sortedResults.end(),
                std::back_inserter(resultVector),
                [](const std::pair<std::u16string, int>& item)
                {
                    return item.first;
                });
        }

        // Return what you have
        return resultVector;
    }

    void Dictionary::addWord(const std::u16string& rWord)
    {
        // Convert to lower case and remember it
        std::u16string lowerWord = rWord;
        WordState wordState = convertToLower(lowerWord);

        // Pointer to node
        Node* pNode = NULL;

        // Pointer to map
        NodeMap* pMap = &mRootMap;

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
                pMap->insert(std::make_pair(c, Node()));
            }

            // Remember current node and new map
            pNode = &(pMap->at(c));
            pMap = &(pNode->children);
        }

        // Set word state in last letter of word, seen from root
        // NONE is initial value
        if (pNode->wordState == WordState::NONE)
        {
            // First occurence, use given case
            pNode->wordState = wordState;
        }
        else if (pNode->wordState != wordState)
        {
            // Already saved with other case or already added as upper and lower case
            pNode->wordState = WordState::BOTH_STARTS;
        }
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
        const std::u16string& rInput,
        uint recursionDepth,
        uint inputPauseDepth,
        uint inputIgnoreDepth,
        std::set<std::u16string>& rFoundWords) const
    {
        // Just pipe it (start the recursion)
        fuzzyWordSearch(
            rInput,
            0,
            u"",
            NULL,
            (int)recursionDepth,
            (int)inputPauseDepth,
            (int)inputIgnoreDepth,
            rFoundWords);
    }

    void Dictionary::fuzzyWordSearch(
        const std::u16string& rInput,
        uint inputStartIndex,
        std::u16string collectedWord,
        Node const * pNode,
        int remainingRecursions,
        int remainingInputPauses,
        int reaminingInputIgnores,
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

        // Consume letters from start index of input word
        uint count = (uint)rInput.size();
        for (uint i = inputStartIndex; i < count; i++)
        {
            // INPUT PAUSES could be implemented here
            // One has to follow all possible entries in map and start there a fuzzy word search
            // and continue with this one. Remind decrementing the remainingInputPauses. Start
            // index for recursion is i-1

            // INPUT IGNORES could be implemented here
            // Just ignore the current letter and go over all map entries and follow them.
            // Difference to pauses is, that start index is i

            // Current letter
            const char16_t& c = rInput[i];

            // "Aaal" -> "Aal" should be found (too many letters)
            if (i > 0 && c == rInput[i-1])
            {
                fuzzyWordSearch(
                    rInput,
                    i + 1,
                    collectedWord,
                    pNode,
                    remainingRecursions,
                    remainingInputPauses,
                    reaminingInputIgnores,
                    rFoundWords);
            }

            // Try to find letter in current map
            NodeMap::const_iterator it = pMap->find(c);
            if (it == pMap->end())
            {
                // Next character was not found. Only add to found words if
                // remaining input pauses are enough to compensate extra letters
                if(count - (i+1) < remainingInputPauses)
                {
                    // Decision whether here is a node is done in add method
                    addFuzzyWord(collectedWord, pNode->wordState, rFoundWords);
                }
                return;
            }
            else
            {
                // Some node was found, so remember that
                pNode = &(it->second);

                // Add letter to collected word
                collectedWord += c;

                // "Al" -> "Aal" should be found  (not enough letters, repeating ones missing)
                fuzzyWordSearch(
                    rInput,
                    i,
                    collectedWord,
                    pNode,
                    remainingRecursions,
                    remainingInputPauses,
                    reaminingInputIgnores,
                    rFoundWords);

                // Only add word when no letters in input are left
                if (i + 1 == count)
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

        // Input word is empty. Now add words which have the collected word as prefix
        if(pNode != NULL)
        {
            // Maximal count of following words for each fully collected
            const uint MAX_FOLLOWING_WORDS = 4;
            addLongerWords(collectedWord, *pNode, MAX_FOLLOWING_WORDS, rFoundWords);
        }
    }

    int Dictionary::addLongerWords(
        const std::u16string& rCollectedWord,
        const Node& rNode,
        int remainingWords,
        std::set<std::u16string>& rFoundWords) const
    {
        if(remainingWords > 0) // should be not necessary
        {
            // Go over children of node and add all words until "remainingWords" counter is too low
            for(const auto& rNodeMapEntry : rNode.children)
            {
                // Collected word
                std::u16string collectedWord = rCollectedWord + rNodeMapEntry.first;

                // Word is only added if here is one (checked by addFuzzyWord method)
                if(addFuzzyWord(collectedWord, rNodeMapEntry.second.wordState, rFoundWords))
                {
                    remainingWords--;
                }

                // Break out of for loop when no more words should be added
                if(remainingWords <= 0) { break; }

                // Do some recursion
                remainingWords = addLongerWords(collectedWord, rNodeMapEntry.second, remainingWords, rFoundWords);

                // Break out of for loop when no more words should be added
                if(remainingWords <= 0) { break; }
            }
        }

        // Return how many words may be added further
        return remainingWords;
    }

    bool Dictionary::addFuzzyWord(const std::u16string& rCollectedWord, WordState collectedState, std::set<std::u16string>& rFoundWords) const
    {
        switch (collectedState)
        {
        case WordState::BOTH_STARTS:
        {
            // Make version with upper case
            std::u16string collectedWordUpper = rCollectedWord;
            firstCharacterToUpper(collectedWordUpper);

            // Add both cases
            rFoundWords.insert(collectedWordUpper);
            rFoundWords.insert(rCollectedWord);
            return true;
        }
        case WordState::UPPER_START:
        {
            // Add upper case word
            std::u16string collectedWordUpper = rCollectedWord;
            firstCharacterToUpper(collectedWordUpper);
            rFoundWords.insert(collectedWordUpper);
            return true;
        }
        case WordState::LOWER_START:
        {
            // Add lower case word
            rFoundWords.insert(rCollectedWord);
            return true;
        }
        default:
        {
            // No word added
            return false;
        }
        }
    }
}
