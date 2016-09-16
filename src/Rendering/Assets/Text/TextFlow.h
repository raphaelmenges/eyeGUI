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

namespace eyegui
{
    // Abstract super class for flow entities
    class FlowEntity
    {
    public:

        // Enumeration of types
        enum class Type { Word, Space, Mark };

        // Virtual destructor
        virtual ~FlowEntity() = 0;

        // Getter for letter count
        virtual uint getLetterCount() const = 0;

        // Getter for type
        Type getType() const { return mType; }

        // Getter for index where entity starts in content
        uint getContentStartIndex() const { return mContentStartIndex; }

        // Getter for index within entities vector
        uint getIndex() const { return mIndex; }

        // Getter for position
        int getX() const { return mX; }
        int getY() const { return mY; }

    protected:

        friend class TextFlow;

        // Members
        Type mType;
        uint mContentStartIndex; // index in content where flow entity starts
        uint mIndex; // index within flow entity vector
        int mX; // relative in flow
        int mY; // relative in flow
    };

    // Class for sub flow word which forms parts of flow word
    class SubFlowWord
    {
    public:

        // Getter for letter count
        uint getLetterCount() const { return (uint)mupWord->lettersXOffsets.size(); }

        // Getter for position
        int getX() const { return mX; }
        int getY() const { return mY; }

        // Getter for pixel width
        int getPixelWidth() const { return mupWord->pixelWidth; }

    protected:

        friend class TextFlow;

        // Members
        int mX; // relative in flow
        int mY; // relative in flow

        // Word
        std::unique_ptr<RenderWord> mupWord; // geometry and information of word
    };

    // Class of flow word which is collection of sub flow words
    class FlowWord : public FlowEntity
    {
    public:

        FlowWord() { mType = FlowEntity::Type::Word; }

        // Getter for sub word count
        uint getSubWordCount() const { return (uint)mSubWords.size(); }

        // Getter for letter count of complete flow word
        virtual uint getLetterCount() const
        {
            uint count = 0;
            for (const auto& rSubWord : mSubWords)
            {
                count += rSubWord->getLetterCount();
            }
            return count;
        }

        // Getter for subword and letter index for given letter offset. Returns whether successfull.
        // Letter index is index inside of sub word
        bool getIndices(uint inputWordIndex, uint& rSubWordIndex, uint& rLetterIndex) const
        {
            // Only continue when enough letters are available
            if(inputWordIndex < getLetterCount())
            {
                int offset = inputWordIndex;
                for(uint i = 0; i < getSubWordCount(); i++)
                {
                    if((offset - mSubWords.at(i)->getLetterCount()) < 0)
                    {
                        rSubWordIndex = i;
                        rLetterIndex = offset;
                        return true;
                    }
                    else
                    {
                        offset -= mSubWords.at(i)->getLetterCount();
                    }
                }
            }
            return false;
        }

        // Get content index by indices. Letter index is within subword
        uint getContentIndex(uint subWordIndex, uint letterIndex) const
        {
            uint index = 0;
            for (uint i = 0; i < subWordIndex; i++)
            {
                index += mSubWords.at(i)->getLetterCount();
            }
            index += letterIndex;
            index += mContentStartIndex;
            return index;
        }

    protected:

        friend class TextFlow;

        // Members
        std::vector<std::unique_ptr<SubFlowWord> > mSubWords; // can be divided into multiple sub words to fit into given space
    };

    // Class for space within text flow
    class FlowSpace : public FlowEntity
    {
    public:

        FlowSpace() { mType = FlowEntity::Type::Space; mCollapsed = false; }

        // Getter for letter count
        virtual uint getLetterCount() const { return 1; }

        // Getter whether collapsed
        bool isCollapsed() const { return mCollapsed; }

        // Getter for pixel width
        float getPixelWidth() const { return mPixelWidth; }

    protected:

        friend class TextFlow;

        // Members
        bool mCollapsed;
        float mPixelWidth;
    };

    // Class for marks like question mark or dot within text flow
    class FlowMark : public FlowEntity
    {
    public:

        FlowMark() { mType = FlowEntity::Type::Mark; }

        // Getter for letter count
        virtual uint getLetterCount() const { return 1; }

        // Getter for pixel width
        float getPixelWidth() const { return mupWord->pixelWidth; }

    protected:

        friend class TextFlow;

        // Members
        std::unique_ptr<RenderWord> mupWord; // geometry and information of word
    };

    // TODO: can be deleted probably later:
    class FlowWord;

    // Text flow class
    class TextFlow : public Text
    {
    public:

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

        // Get flow height (interesting if overflowHeight is true)
        int getFlowHeight() const;

		// Get pixel width of space letter in used font
        float getPixelWidthOfSpace() const;

        // Get count of flow entities
        uint getFlowEntityCount() const;

        // Get content at given index plus length
        std::u16string getContent(uint index, uint letterCount) const;

        // Get weak pointer to flow entity at index. Maybe empty when index not available
        std::weak_ptr<const FlowEntity> getFlowEntity(uint index) const;

        // Use position in flow coordinates to get an entity. Maybe empty when index not available.
        // Coordinates relative in pixel space of flow
        std::weak_ptr<const FlowEntity> getFlowEntity(int x, int y) const;






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
            float lineHeight,
            RenderWordVertices& rVertices);

        // Calculate word with maximal width (in doubt split it). If result is empty, not enough space available
        std::vector<RenderWord> calculateFitWord(std::u16string content, int maxPixelWidth, float scale) const;

        // Inserts word into vector, returns true at success
        bool insertFitWord(std::vector<RenderWord>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale) const;

        // Classify letter in terms of entity type
        FlowEntity::Type classifyLetter(const char16_t& rLetter) const;

        // Members
        TextFlowAlignment mAlignment;
        TextFlowVerticalAlignment mVerticalAlignment;
        int mWidth;
        int mHeight;
		int mFlowWidth;
        int mFlowHeight;
		float mPixelOfSpace;
		bool mOverflowHeight; // when overflow height, height in transformAndSize is ignored and overwritten by height necessary to display complete text
        std::vector<std::shared_ptr<FlowEntity> > mFlowEntities; // holding information to make text manipulation possible
	};
}

#endif // TEXT_FLOW_H_
