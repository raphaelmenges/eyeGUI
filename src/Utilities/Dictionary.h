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
// - Maybe do not use max count in fuzzy search for termination. Do full run and then decide which words to take using classificator.

// TODO:
// - Multiple dictionaries (URLs, Language....) -> vector of dicts in GUI
// - "Wissenschaft" returns two times "Wissenschaft"
//   - Solution:
/*
		sort( vec.begin(), vec.end() );
		vec.erase( unique( vec.begin(), vec.end() ), vec.end() );
*/

namespace eyegui
{
	class Dictionary
	{
	public:

		// Constructor
		Dictionary();

		// Destructor
		virtual ~Dictionary();

		// Check for exact word
		bool checkForWord(const std::u16string& rWord) const;

		// Give similar words. May return empty vector or similar words, ordered by probability (more or less)
		std::vector<std::u16string> similarWords(const std::u16string& rWord, uint maxCount) const;

	private:

		// Some typedef for the map
		class Node;
		typedef std::map<char16_t, Node> NodeMap;

		// Possible states for word, using 1 byte char as structure. None is used for "here is no word"
		enum class WordState : char { NONE, LOWER_START, UPPER_START, BOTH_STARTS };

		// Add single word to dictionary
		void addWord(const std::u16string& rWord);

		// Convert to lower case. Returns word state
		WordState convertToLower(std::u16string& rWord) const;

		// DOES NOT CHECK WHETHER GIVEN VECTOR HAS ALREADY ENOUGH WORDS
		// Fuzzy search for words. Takes lower case word as input. Returns true when max count reached
		bool fuzzyWordSearch(const std::u16string& rLowerWord, bool originalWordStartsUpperCase, uint maxCount, std::vector<std::u16string>& rFoundWords) const;
		bool fuzzyWordSearch(const std::u16string& rLowerWord, uint wordStartIndex, bool originalWordStartsUpperCase, std::u16string collectedWord, Node const * pNode, uint maxCount, std::vector<std::u16string>& rFoundWords) const;

		// DOES NOT CHECK WHETHER GIVEN VECTOR HAS ALREADY ENOUGH WORDS
		// Add fuzzy word to found words in fuzzy search. Returns true when enough words found
		bool addFuzzyWord(const std::u16string rCollectedWord, WordState collectedState, bool originalWordStartsUpperCase, uint maxCount, std::vector<std::u16string>& rFoundWords) const;
		
		// Inner class for node. One node per letter in each word. Buildung up a tree by reusing existing nodes
		class Node
		{
		public:

			// Constructor
			Node(char16_t letter) : letter(letter), wordState(WordState::NONE) {}

			// Members
			char16_t letter; // Letter in node
			WordState wordState; // State of word formed by letters in nodes from root to here
			NodeMap children; // Further possible letters to form other words
		};

		// Members
		std::map<char16_t, Node> mRootMap; // Root map
	};
}

#endif // DICTIONARY_H_
