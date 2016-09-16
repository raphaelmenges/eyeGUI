//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Entities which form a text flow.

#ifndef FLOW_ENTITY_H_
#define FLOW_ENTITY_H_

#include "src/Rendering/Assets/Text/Text.h"

#include <algorithm>

namespace eyegui
{
    // Class for sub flow word which forms parts of flow word
    class SubFlowWord
    {
    public:

        // Getter for letter count
        int getLetterCount() const { return (int)upWord->lettersXOffsets.size(); }

        // Members
        int x; // relative in flow
        int y; // relative in flow

        // Word
        std::unique_ptr<RenderWord> upWord; // geometry and information of word
    };

    // Abstract super class for flow entities
    class FlowEntity
    {
    public:

        // Enumeration of types
        enum class Type { Word, Space };

        // Getter for letter count
        virtual int getLetterCount() const = 0;

        // Getter for type
        Type getType() const { return type; }

        // Members
        Type type;
        int contentStartIndex; // index in content where flow space starts
        int index; // index within flow entity vector
    };

    // Class of flow word which is collection of sub flow words
    class FlowWord : public FlowEntity
    {
    public:

        FlowWord() { type = FlowEntity::Type::Word;  }

        // Getter for sub word count
        int getSubWordCount() const { return (int)subWords.size(); }

        // Getter for letter count of complete flow word
        virtual int getLetterCount() const
        {
            int count = 0;
            for (const auto& rSubWord : subWords)
            {
                count += rSubWord->getLetterCount();
            }
            return count;
        }

        // Getter for subword and letter index for given letter offset. Returns whether successfull.
        // Letter index is index inside of sub word
        bool getIndices(int wordLetterIndex, int& rSubWordIndex, int& rLetterIndex) const
        {
            // Only continue when enough letters are available
            if(wordLetterIndex < getLetterCount())
            {
                int offset = wordLetterIndex;
                for(int i = 0; i < getSubWordCount(); i++)
                {
                    if((offset - subWords.at(i)->getLetterCount()) < 0)
                    {
                        rSubWordIndex = i;
                        rLetterIndex = offset;
                        return true;
                    }
                    else
                    {
                        offset -= subWords.at(i)->getLetterCount();
                    }
                }
            }
            return false;
        }

        // Get content index by indices
        int getContentIndex(int subWordIndex, int letterIndex) const
        {
            int index = 0;
            for (int i = 0; i < subWordIndex; i++)
            {
                index += subWords.at(i)->getLetterCount();
            }
            index += letterIndex;
            index += contentStartIndex;
            return index;
        }

        // Members
        int contentStartIndex; // index in content where flow word starts
        std::vector<std::unique_ptr<SubFlowWord> > subWords; // can be divided into multiple sub words to fit into given space
    };

    // Class for space within text flow
    class FlowSpace : public FlowEntity
    {
    public:

        FlowSpace() { type = FlowEntity::Type::Space; collapsed = false; }

        // Getter for letter count
        virtual int getLetterCount() const { return 1; }

        // Members
        bool collapsed;
    };
}

#endif // FLOW_ENTITY_H_
