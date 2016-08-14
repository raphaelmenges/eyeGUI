//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Creates geometry for given font and text content. Is delegated by elements
// for font rendering in general. All values in this class are in pixel space,
// even floats. Internally the origin is at lower left but all methods
// expect values in a coordinate system with an origin at the upper left.

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "src/Rendering/Assets/Text/Text.h"

#include <algorithm>

namespace eyegui
{
    class TextFlow : public Text
    {
    public:

        // Struct like class for sub flow word (if word is devided to fit into lines)
        class SubFlowWord
		{
        public:

            // Getter for letter count
            int getLetterCount() const { return (int)lettersXOffsets.size(); }

            // Members
			int x; // relative in flow
			int y; // relative in flow
			int width; // pixel width
			// Height is given by line height

			// Vector which saves the x offsets of single letters inside sub word
			std::vector<int> lettersXOffsets; // position after letter
		};

        // Struct like class for word in flow which is used to get a word at given coordinate in text flow
        class FlowWord
		{
        public:

            // Getter for sub word count
            int getSubWordCount() const { return (int)subWords.size(); }

            // Getter for letter count of complete flow word
            int getLetterCount() const
            {
                int count = 0;
                std::for_each(subWords.begin(), subWords.end(), [&count](const SubFlowWord& rSubWord) { count += rSubWord.getLetterCount(); });
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
                        if((offset - subWords.at(i).getLetterCount()) < 0)
                        {
                            rSubWordIndex = i;
                            rLetterIndex = offset;
                            return true;
                        }
                        else
                        {
                            offset -= subWords.at(i).getLetterCount();
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
					index += subWords.at(i).getLetterCount();
				}
				index += letterIndex;
				index += contentIndex;
				return index;
			}

            // Members
			int contentIndex; // index in content where flow word starts
			std::vector<SubFlowWord> subWords; // can be divided into multiple sub words to fit into given space
			int index; // index in text flow's vector for readdressing
		};

        // Constructor
        TextFlow(
            GUI const * pGUI,
            AssetManager* pAssetManager,
            Font const * pFont,
            FontSize fontSize,
            TextFlowAlignment alignment,
            TextFlowVerticalAlignment verticalAlignment,
            float scale,
            std::u16string content,
			bool overflowHeight);

        // Destructor
        virtual ~TextFlow();

        // Transform and size (has to be called before first usage)
        void transformAndSize(
            int x,
            int y,
            int width,
            int height);

        // Draw
        virtual void draw(
			glm::vec4 color,
			float alpha,
			bool renderBackground = false,
			int xOffset = 0,
			int yOffset = 0) const;

		// Get height (interesting if overflowHeight is true)
		int getHeight() const { return mHeight; }

		// Get pixel width of space letter in used font
		float getPixelWidthOfSpace() const { return mPixelOfSpace; }

        // Get count of flow words
        int getFlowWordCount() const { return (int)mFlowWords.size(); }

		// Get data of certain word by index. Returns false if not found.
		// Position is given in flow coordinates
		bool getFlowWord(int index, FlowWord& rFlowWord) const;

		// Use position in flow coordinates to find a word. Returns false if not found
		bool getFlowWord(int x, int y, FlowWord& rFlowWord) const;

        // Get flow word, sub word index and letter index by content index. Returns whether successfull.
        // rLetterIndex of -1 symbolizes position in front of first letter
        bool getFlowWordAndIndices(int contentIndex, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex) const;

        // Insert content after index of exisiting content.
        // Fills resulting information about flow word, sub word index and letter index at end of insertion.
        // Returns whether successull
        bool insertContent(int index, std::u16string content, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex);

        // Erases content from index to end index, including index and excluding index + length. Returns whether succesfully
        bool eraseContent(int index, int letterCount, FlowWord& rFlowWord, int& rSubWordIndex, int& rLetterIndex);

    protected:

        // Calculate mesh (in pixel coordinates)
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight, std::vector<glm::vec3>& rVertices,
            std::vector<glm::vec2>& rTextureCoordinates);

        // Calculate word with maximal width (in doubt split it). If result is empty, not enough space available
        std::vector<Word> calculateFitWord(std::u16string content, int maxPixelWidth, float scale) const;

        // Inserts word into vector, returns true at success
        bool insertFitWord(std::vector<Word>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale) const;

        // Members
        TextFlowAlignment mAlignment;
        TextFlowVerticalAlignment mVerticalAlignment;
        int mWidth;
        int mHeight;
		int mFlowWidth;
        int mFlowHeight;
		float mPixelOfSpace;
		bool mOverflowHeight; // when overflow height, height in transformAndSize is ignored and overwritten by height necessary to display complete text
		std::vector<FlowWord> mFlowWords; // holding information to make text manipulation possible
	};
}

#endif // TEXT_FLOW_H_
