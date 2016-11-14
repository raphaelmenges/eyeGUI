//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Creates geometry for given font and text content. Is delegated by elements
// for font rendering in general. All values in this class are in pixel space,
// even floats. Internally the origin is at lower left but all methods
// expect values in a coordinate system with an origin at the upper left.
// Front of content is marked with -1. Flow parts are addressed with
// 0..flowPartCount-1. Letter indices and offset indices are defined in a
// range from -1 to letterCount-1, where -1 marks the front of a flow part.

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "src/Rendering/Assets/Text/Text.h"

namespace eyegui
{
    // Class which forms parts of flow entity
    class FlowPart
    {
    public:

        // Getter for position
        int getX() const { return mX; }
        int getY() const { return mY; }

        // Getter for letter count
        uint getLetterCount() const
        {
            if(mupWord != nullptr)
            {
                return mupWord->getLetterCount();
            }
            else
            {
                return 1;
            }
        }

        // Getter for pixel width
        float getPixelWidth() const
        {
            if(mupWord != nullptr)
            {
                return mupWord->pixelWidth;
            }
            else
            {
                return mPixelWidth;
            }

        }

        // Getter whether collapsed
        bool isCollapsed() const { return mCollapsed; }

		// Getter for x offset of letters. Returns 0 if offset not found.
		// Offset index starts at -1, where this symbolizes the position in front of
		// the first letter. Index 0 is the position after the the first letter
		// and so on... so there are letterCount + 1 many offsets
		int getXOffset(uint offsetIndex) const
		{
			// Increment offset index to fit render words implementation
			offsetIndex++;

			if (mupWord != nullptr)
			{
				if (offsetIndex < mupWord->xOffsets.size()) // index found
				{
					return mupWord->xOffsets.at(offsetIndex);
				}
				else // not found
				{
					return 0;
				}
				
			}
			else // no render word
			{
				// Fix for space: Since each space is represented by one flow part, return pixel width when index is 1
				if (offsetIndex == 1)
				{
					return (int)getPixelWidth();
				}
				else
				{
					return 0;
				}
			}
		}

    protected:

		// TextFlow fill values
        friend class TextFlow;

		// Constructor
		FlowPart()
		{
			mX = 0;
			mY = 0;
			mCollapsed = false;
			mupWord = nullptr;
			mPixelWidth = 0;
		}

        // Members
        int mX; // relative in flow
        int mY; // relative in flow
        bool mCollapsed;
        std::unique_ptr<RenderWord> mupWord; // geometry information of word
        float mPixelWidth; // fallback when mupWord is nullptr
    };

    // Class for flow entities. Consists at least of one flow part
    class FlowEntity
    {
    public:

        // Enumeration of types
        enum class Type { Word, Space, Mark };

        // Getter for type
        Type getType() const { return mType; }

        // Getter for index where entity starts in content
        uint getContentStartIndex() const { return mContentStartIndex; }

        // Getter for index of flow entity within entities vector of text flow object
        uint getIndex() const { return mIndex; }

		// Getter for parts. Returns emtpy weak pointer at failure
		std::weak_ptr<const FlowPart> getFlowPart(uint index) const
		{
			if (index < getFlowPartCount())
			{
				return mFlowParts.at(index);
			}
			else
			{
				return std::weak_ptr<const FlowPart>();
			}
		}

        // Getter for flow part count
        uint getFlowPartCount() const { return (uint)mFlowParts.size(); }

        // Getter for letter count of complete flow entity
        uint getLetterCount() const
        {
            uint count = 0;
            for (const auto& rFlowPart : mFlowParts)
            {
                count += rFlowPart->getLetterCount();
            }
            return count;
        }

		// Calculates index of flow part and letter within flow part of given letter within the represented word.
		// -1 means in front of the word, everything else within. Returns whether successful
		bool getIndices(int wordLetterIndex, uint& rFlowPartIndex, int& rLetterIndex) const
		{
			// Case when in front of word
			if (wordLetterIndex == -1)
			{
				rFlowPartIndex = 0;
				rLetterIndex = -1;
				return true;
			}
			else if(wordLetterIndex >= 0)
			{
				int localIndex = (int) wordLetterIndex;

				// Go over flow parts
				for (uint i = 0; i < getFlowPartCount(); i++)
				{
					// Get length of flow part
					int partLetterCount = mFlowParts.at(i)->getLetterCount();

					// Check whether index is already included
					if (localIndex < partLetterCount) // add one because front is zero
					{
						rFlowPartIndex = i;
						rLetterIndex = localIndex;
						return true;
					}
					else
					{
						localIndex -= partLetterCount; // subtract letter count from local index
					}
				}
			}

			// Fallback
			return false;
		}

        // Get content index by indices
        int getContentIndex(uint flowPartIndex, int letterIndex) const
        {
            int index = 0;
            for (uint i = 0; i < flowPartIndex; i++)
            {
                index += mFlowParts.at(i)->getLetterCount();
            }
            index += letterIndex;
            index += mContentStartIndex;
            return index;
        }

    protected:

		// TextFlow fill values
        friend class TextFlow;

		// Constructor
		FlowEntity()
		{
			mContentStartIndex = 0;
			mIndex = 0;
			mX = 0;
			mY = 0;
		}

		// Add all flow parts pixel width, not regarding the possible placing in different lines.
		// Used in calculation of text flow mesh
		float getPixelWidth() const
		{
			float pixelWidth = 0;
			for (const auto& rFlowPart : mFlowParts)
			{
				pixelWidth += rFlowPart->getPixelWidth();
			}
			return pixelWidth;
		}

		// Getter for position within text flow.
		// Is equal to position of first flow part
		int getX() const { return mX; }
		int getY() const { return mY; }

        // Members
        int mX; // relative in flow
        int mY; // relative in flow
        Type mType;
        uint mContentStartIndex; // index in content where flow entity starts
        uint mIndex; // index within flow entity vector
        std::vector<std::shared_ptr<FlowPart> > mFlowParts;
    };

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
			bool overflowHeight,
			bool collapseSpaces);

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

		// Get complete content
		std::u16string getContent() const;

        // Get content at given index plus length
        std::u16string getContent(uint index, uint letterCount) const;

        // Get weak pointer to flow entity at index. May be empty when index not available
        std::weak_ptr<const FlowEntity> getFlowEntity(uint index) const;

        // Use position in flow coordinates to get an entity. Maybe empty when index not available.
        // Coordinates relative in pixel space of flow
        std::weak_ptr<const FlowEntity> getFlowEntity(int x, int y) const;

        // Get flow entity, flow part index and letter index inside flow part. Maybe empty when index not available.
        // Content index of -1 indicates front of text.
        std::weak_ptr<const FlowEntity> getFlowEntityAndIndices(
            int contentIndex,
            uint& rFlowPartIndex,
            int& rLetterIndex) const;

        // Insert content after index of exisiting content.
        // Returns flow entity and sets indices to position after insertion
        std::weak_ptr<const FlowEntity> insertContent(uint index, std::u16string content, uint& rFlowPartIndex, int& rLetterIndex);

        // Erases letterCount many letters from content, beginning and including index and excluding index + letterCount.
        // Negative index results in deletion of letters before index.
        // Returns flow entity and sets indices to front position of deletion
        std::weak_ptr<const FlowEntity> eraseContent(int index, int letterCount, uint& rFlowPartIndex, int& rLetterIndex);

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
		bool mCollapseSpaces; // collapses spaces at frond and end of line
	};
}

#endif // TEXT_FLOW_H_
