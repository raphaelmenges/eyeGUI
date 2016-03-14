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
		/*
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
		std::cout << checkForWord(u"haus") << std::endl;

		std::cout << "Dictionary finished!" << std::endl;
		*/
	}

	Dictionary::~Dictionary()
	{
		// Nothing to do
	}

	bool Dictionary::checkForWord(const std::u16string& rWord) const
	{
		// TODO: just a rough test (cases!)

		// Convert to lower case and remember it
		WordCase wordCase = WordCase::LOWER;
		std::u16string lowerWord = toLower(rWord);
		if (lowerWord != rWord)
		{
			wordCase = WordCase::UPPER;
		}

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
			if (pLastNode->wordCase == wordCase || pLastNode->wordCase == WordCase::BOTH)
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
		WordCase wordCase = WordCase::LOWER;
		std::u16string lowerWord = toLower(rWord);
		if (lowerWord != rWord)
		{
			wordCase = WordCase::UPPER;
		}

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
		if (pLastNode->wordCase == WordCase::NONE)
		{
			pLastNode->wordCase = wordCase;
		}
		else if (pLastNode->wordCase != wordCase)
		{
			pLastNode->wordCase = WordCase::BOTH;
		}
	}

	std::u16string Dictionary::toLower(const std::u16string& rWord) const
	{
		// Convert to lower case and remember (DOES NOT WORK BECAUSE UNICODE SUPPORT SUCKS)
		std::u16string lowerWord(rWord);
		/*std::locale::global(std::locale(std::locale(), new std::ctype<char16_t>()));
		std::use_facet<std::ctype<char16_t> >(std::locale()).tolower(&lowerWord[0], &lowerWord[0] + lowerWord.size()); */

		// Return lower word
		return lowerWord;
	}
}