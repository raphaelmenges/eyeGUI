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
		std::ifstream in("C:/Users/Raphael/Desktop/ger.txt");

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
		std::cout << "CheckForWord 1: " << checkForWord(u"Ärger") << std::endl;
		std::cout << "CheckForWord 2: " << checkForWord(u"ärger") << std::endl;

		// Fuzzy word search
		auto testA = similarWords(u"Wisenschafft", 5);
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

		// Create vector to collect results
		std::vector<std::u16string> results;

		// Search for given word, ignore identical letters in one row
		fuzzyWordSearch(lowerWord, wordState == WordState::UPPER_START, results);

		// TODO: 
		// - check whether already enough words
		// - Maybe do not use max count in fuzzy search for termination. Do full run and then decide which words to take using classificator

		// Return what you have (could be nothing)
		return results;
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

	void Dictionary::fuzzyWordSearch(const std::u16string& rLowerWord, bool originalWordStartsUpperCase, std::vector<std::u16string>& rFoundWords) const
	{
		// Just pipe it (start the recursion)
		fuzzyWordSearch(rLowerWord, 0, originalWordStartsUpperCase, u"", NULL, rFoundWords);
	}

	void Dictionary::fuzzyWordSearch(const std::u16string& rLowerWord, uint wordStartIndex, bool originalWordStartsUpperCase, std::u16string collectedWord, Node const * pNode, std::vector<std::u16string>& rFoundWords) const
	{
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
					fuzzyWordSearch(rLowerWord, i + 1, originalWordStartsUpperCase, collectedWord, pNode, rFoundWords);
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
				fuzzyWordSearch(rLowerWord, i, originalWordStartsUpperCase, collectedWord, pNode, rFoundWords);
				
				// Only add word when no letters in input are left
				if (i + 1  == count)
				{
					// Adds word to found words
					addFuzzyWord(collectedWord, pNode->wordState, originalWordStartsUpperCase, rFoundWords);
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
	}

	void Dictionary::addFuzzyWord(const std::u16string rCollectedWord, WordState collectedState, bool originalWordStartsUpperCase, std::vector<std::u16string>& rFoundWords) const
	{
		// Decide how to add word
		if (collectedState == WordState::BOTH_STARTS)
		{
			// Make version with upper case
			std::u16string collectedWordUpper = rCollectedWord;
			firstCharacterToUpper(collectedWordUpper);

			// Add both cases
			if (originalWordStartsUpperCase)
			{
				// First upper case, then lower
				rFoundWords.push_back(collectedWordUpper);
				rFoundWords.push_back(rCollectedWord);
			}
			else
			{
				// First lower case, than upper
				rFoundWords.push_back(rCollectedWord);
				rFoundWords.push_back(collectedWordUpper);
			}
		}
		else
		{
			// Add what is there
			if (collectedState == WordState::UPPER_START)
			{
				// Add upper case word
				std::u16string collectedWordUpper = rCollectedWord;
				firstCharacterToUpper(collectedWordUpper);
				rFoundWords.push_back(collectedWordUpper);
			}
			else
			{
				rFoundWords.push_back(rCollectedWord);
			}
		}
	}
}