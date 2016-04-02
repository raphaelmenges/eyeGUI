//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Dictionary.h"

#include "src/Utilities/OperationNotifier.h"
#include "src/Utilities/Helper.h"
#include "src/Utilities/PathBuilder.h"
#include "externals/levenshtein-sse/levenshtein-sse.hpp"

#include <fstream>
#include <algorithm>
#include <sstream>

namespace eyegui
{
    Dictionary::Dictionary(std::string filepath)
    {
        // Read file with instream
        std::ifstream in(buildPath(filepath).c_str());

        // Build up dictionary
        if(in)
        {
            // Convert input file to string
            std::stringstream strStream;
            strStream << in.rdbuf();
            std::string content = strStream.str();

            // Close file
            in.close();

            // Streamline line endings
            streamlineLineEnding(content, true);

            // Add words to dictionary
            std::string delimiter = "\n";
            size_t pos = 0;
            std::string line;
            while ((pos = content.find(delimiter)) != std::string::npos)
            {
                // Extract line
                line = content.substr(0, pos);
                content.erase(0, pos + delimiter.length());

                // Convert line to utf-16 string
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
        else
        {
            throwError(OperationNotifier::Operation::DICTIONARY, "Dictionary file not found", filepath);
        }
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
        if (pNode != NULL && (pNode->wordState == wordState || pNode->wordState == WordState::BOTH_STARTS))
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    std::vector<std::u16string> Dictionary::similarWords(const std::u16string& rWord, bool makeFirstLetterUpperCase) const
    {
        // Convert to lower case
        std::u16string lowerWord = rWord;
        convertToLower(lowerWord); // state is not necessary to remember
        uint wordLength = (uint)rWord.size();

        // Create set to collect results
        std::set<std::u16string> resultSet;

        // Search for given word, ignore identical letters in one row
        fuzzyWordSearch(
            lowerWord,
            DICTIONARY_INPUT_REPEAT_IGNORE_DEPTH,
            DICTIONARY_INPUT_REPEAT_PAUSE_DEPTH,
            DICTIONARY_INPUT_PAUSE_DEPTH,
            DICTIONARY_INPUT_IGNORE_DEPTH,
            resultSet);

        // Save size of result set
        int sizeOfResultSet = (int)resultSet.size();

        // Only continue when necessary
        std::vector<std::u16string> resultVector;
        if(sizeOfResultSet > 0)
        {
            // Rate results by word distance
            int distance;
            std::vector<std::pair<std::u16string, int> > sortedResults;
            sortedResults.reserve(sizeOfResultSet);
            std::u16string comparsionWord = lowerWord;
            firstCharacterToUpper(comparsionWord);
            for(const std::u16string& rFoundWord : resultSet)
            {
                // Determine common size of both words to not discriminate appended letters
                uint commonLength = std::min(wordLength, (uint)rFoundWord.size());

                // Use input with possible first upper letter as comparsion
                // Since in the structure is only the case of the first letter saved
                distance = (int)levenshteinSSE::levenshtein(
                    comparsionWord.begin(), comparsionWord.begin() + commonLength,
                    rFoundWord.begin(), rFoundWord.begin() + commonLength);

                // Build up structure with found word and distance to searched one
                sortedResults.push_back(std::make_pair(rFoundWord, distance));
            }

            // Sort the built structure
            std::sort(
                sortedResults.begin(),
                sortedResults.end(),
                [&comparsionWord](const std::pair<std::u16string, int>& left, const std::pair<std::u16string, int>& right)
                {
                    // Collect information
                    int distanceLeft = left.second;
                    int distanceRight = right.second;
                    bool leftFirstIdentical = false;
                    bool leftLastIdentical = false;
                    bool rightFirstIdentical = false;
                    bool rightLastIdentical = false;

                    if (!comparsionWord.empty())
                    {
                        if (!left.first.empty())
                        {
                            leftFirstIdentical = left.first.front() == comparsionWord.front();
                            leftLastIdentical = left.first.back() == comparsionWord.back();
                        }
                        if (!right.first.empty())
                        {
                            rightFirstIdentical = right.first.front() == comparsionWord.front();
                            rightLastIdentical = right.first.back() == comparsionWord.back();
                        }
                    }

                    // Decide which word is the better suggestion (just some random stuff which looks good)
                    if (leftFirstIdentical && leftLastIdentical)
                    {
                        // Integer division
                        distanceLeft /= 2;
                    }
                    if (rightFirstIdentical && rightLastIdentical)
                    {
                        // Integer division
                        distanceRight /= 2;
                    }

                    return distanceLeft < distanceRight;
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

            // Make first letter upper case if wished
            if(makeFirstLetterUpperCase)
            {
                std::for_each(resultVector.begin(), resultVector.end(), firstCharacterToUpper);
            }
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
        if (pNode != NULL)
        {
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
        uint inputRepeatIgnoreDepth,
        uint inputRepeatPauseDepth,
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
            (int)inputRepeatIgnoreDepth,
            (int)inputRepeatPauseDepth,
            (int)inputPauseDepth,
            (int)inputIgnoreDepth,
            true,
            rFoundWords);
    }

    void Dictionary::fuzzyWordSearch(
        const std::u16string& rInput,
        uint inputStartIndex,
        std::u16string collectedWord,
        Node const * pNode,
        int remainingInputRepeatIgnores,
        int remainingInputRepeatPauses,
        int remainingInputPauses,
        int reaminingInputIgnores,
        bool mayAddLongerWords,
        std::set<std::u16string>& rFoundWords) const
    {
        // Pointer to root map as fallback
        NodeMap const * pMap = &mRootMap;

        // Get pointer to map provided by node
        if (pNode != NULL)
        {
            // Use the the map given in node
            pMap = &(pNode->children);
        }

        // Consume letters from start index of input word
        uint count = (uint)rInput.size();
        uint i = inputStartIndex; // Must be visible outside of loop
        for (; i < count; i++)
        {
            // Suspect input to be incomplete ("Hus" -> "Haus")
            if(remainingInputPauses > 0)
            {
                for(const auto& rCharNode : *pMap)
                {
                    fuzzyWordSearch(
                        rInput,
                        i,
                        collectedWord + rCharNode.first,
                        &(rCharNode.second),
                        0,
                        0,
                        remainingInputPauses - 1,
                        reaminingInputIgnores,
                        false,
                        rFoundWords);
                }
            }

            // Ignore some input ("Huus" -> "Haus")
            if(reaminingInputIgnores > 0)
            {
                // Just ignore given letter from input, add letter from map and go on
                for(const auto& rCharNode : *pMap)
                {
                    fuzzyWordSearch(
                        rInput,
                        i + 1,
                        collectedWord + rCharNode.first,
                        &(rCharNode.second),
                        0,
                        0,
                        remainingInputPauses,
                        reaminingInputIgnores - 1,
                        false,
                        rFoundWords);
                }
            }

            // Current letter
            const char16_t& c = rInput[i];

            // "Aaal" -> "Aal" should be found (too many letters)
            if (i > 0 && remainingInputRepeatIgnores > 0 && c == rInput[i-1])
            {
                fuzzyWordSearch(
                    rInput,
                    i + 1,
                    collectedWord,
                    pNode,
                    remainingInputRepeatIgnores -1,
                    remainingInputRepeatPauses,
                    remainingInputPauses,
                    reaminingInputIgnores,
                    mayAddLongerWords,
                    rFoundWords);
            }

            // Try to find letter in current map
            NodeMap::const_iterator it = pMap->find(c);
            if (it == pMap->end())
            {
                // Next character was not found. Only add to found words if
                // remaining input pauses are enough to compensate extra letters
                if(pNode != NULL && (int)count - (int)(i+1) < remainingInputPauses)
                {
                    // Decision whether here is a node is done in add method
                    addFuzzyWord(collectedWord, pNode->wordState, rFoundWords);
                }

				// Return from this recursion
                return;
            }
            else
            {
                // Some node was found since iterator not at end of map, so remember that
                pNode = &(it->second);

                // Add letter to collected word
                collectedWord += c;

                // "Al" -> "Aal" should be found  (not enough letters, repeating ones missing)
                if(remainingInputRepeatPauses > 0)
                {
                    fuzzyWordSearch(
                        rInput,
                        i,
                        collectedWord,
                        pNode,
                        remainingInputRepeatIgnores,
                        remainingInputRepeatPauses - 1,
                        remainingInputPauses,
                        reaminingInputIgnores,
                        mayAddLongerWords,
                        rFoundWords);
                }

				// Prepare next iteration
				pMap = &(pNode->children);
            }
        }

		// Only continue, if some node from map was chosen
		if (pNode != NULL)
		{
			// Add current word when no letters in input are left
			if (i >= count)
			{
				// Adds word to found words
				addFuzzyWord(collectedWord, pNode->wordState, rFoundWords);

				// This constraint means, that no words shorter than input can be added
				// (but words like "Aaal" because of DICTIONARY_INPUT_REPEAT_IGNORE_DEPTH)
			}

			// Add words which have the collected word as prefix, if wished
			if (mayAddLongerWords)
			{
				addLongerWords(collectedWord, *pNode, DICTIONARY_MAX_FOLLOWING_WORDS, rFoundWords);
			}
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
