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

		// Testing whether words can be found
		std::cout << "Start dictionary tests!" << std::endl;

		std::cout << checkForWord(u"Ärger") << std::endl;
		std::cout << checkForWord(u"ärger") << std::endl;

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

		// Pointer to root map
		NodeMap const * pMap = &mRootMap;

		// Pointer to last node
		Node const * pLastNode = NULL;

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
					pLastNode = &(it->second);
					pMap = &(it->second.children);
				}
			}
		}

		// Can it happen?
		if (pLastNode != NULL)
		{
			if (pLastNode->wordState == wordState || pLastNode->wordState == WordState::BOTH_STARTS)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	std::vector<std::u16string> Dictionary::similarWords(const std::u16string& rWord, uint count) const
	{
		// TODO
		return std::vector<std::u16string>();
	}

	void Dictionary::addWord(const std::u16string& rWord)
	{
		// Convert to lower case and remember it
		std::u16string lowerWord = rWord;
		WordState wordState = convertToLower(lowerWord);

		// Pointer to root map
		NodeMap* pMap = &mRootMap;

		// Pointer to last node
		Node* pLastNode = NULL;

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
			pLastNode = &(pMap->at(c));
			pMap = &(pLastNode->children);
		}

		// Set word case in last letter
		// NONE is initial value
		if (pLastNode->wordState == WordState::NONE)
		{
			pLastNode->wordState = wordState;
		}
		else if (pLastNode->wordState != wordState)
		{
			pLastNode->wordState = WordState::BOTH_STARTS;
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
}