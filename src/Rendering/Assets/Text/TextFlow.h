//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Creates geometry for given font and text content. Is delegated by elements
// for font rendering in general. All values in this class are in pixel space,
// even floats. Internally the origin is at lower left but all methods
// expect values in a coordinate system with an origin at the upper left.
// Front of content is marked with -1. Front of flow parts is marked by 0,
// so a flow part has letterCount + 1 indices.

#ifndef TEXT_FLOW_H_
#define TEXT_FLOW_H_

#include "src/Rendering/Assets/Text/Text.h"

namespace eyegui
{
    // Class which forms parts of flow entity
    class FlowPart
    {
    public:

        // Constructor
        FlowPart()
        {
            mX = 0;
            mY = 0;
            mCollapsed = false;
            mupWord = nullptr;
            mPixelWidth = 0;
        }

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

		// Getter for x offset of letter. Returns -1 if letter not found.
		// Letter index starts at 0, where this symbolizes the position before
		// the first letter. Index 1 is the position after the the first letter
		// and so on...
		int getLetterXOffset(uint letterIndex) const
		{
			if (letterIndex < mupWord->lettersXOffsets.size())
			{
				return mupWord->lettersXOffsets.at(letterIndex);
			}
		}

    protected:

        friend class TextFlow;

        // Members
        int mX; // relative in flow
        int mY; // relative in flow
        bool mCollapsed;
        std::unique_ptr<RenderWord> mupWord; // geometry and information of word
        float mPixelWidth; // fallback when mupWord is null
    };

    // Class for flow entities
    class FlowEntity
    {
    public:

        // Enumeration of types
        enum class Type { Word, Space, Mark };

        // Constructor
        FlowEntity()
        {
            mContentStartIndex = 0;
            mIndex = 0;
            mX = 0;
            mY = 0;
        }

        // Getter for type
        Type getType() const { return mType; }

        // Getter for index where entity starts in content
        uint getContentStartIndex() const { return mContentStartIndex; }

        // Getter for index of flow entity within entities vector
        uint getIndex() const { return mIndex; }

        // Getter for position
        int getX() const { return mX; }
        int getY() const { return mY; }

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

		// Calculates index of flow part and letter within flow part of given letter within a raw word.
		// Zero means in front of the word, everything else within
		bool getIndices(uint wordLetterIndex, uint& rFlowPartIndex, uint& rLetterIndex) const
		{
			// Case when in front of word
			if (wordLetterIndex == 0)
			{
				rFlowPartIndex = 0;
				rLetterIndex = 0;
				return true;
			}
			else
			{
				int localIndex = (int) wordLetterIndex - 1; // subtract front position

				// Go over flow parts
				for (uint i = 0; i < getFlowPartCount(); i++)
				{
					// Get length of flow part
					int partLetterCount = mFlowParts.at(i)->getLetterCount();

					// Check whether index is already included
					if (localIndex < partLetterCount) // add one because front is zero
					{
						rFlowPartIndex = i;
						rLetterIndex = localIndex + 1; // shift one to the right to never be in front of the flow part
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

        // Get content index by indices. Letter index is within flow part
        uint getContentIndex(uint flowPartIndex, uint letterIndex) const
        {
            uint index = 0;
            for (uint i = 0; i < flowPartIndex; i++)
            {
                index += mFlowParts.at(i)->getLetterCount();
            }
            index += letterIndex - 1; // because letter index is including front position
            index += mContentStartIndex;
            return index;
        }

        // Add all flow parts pixel width, not regarding the possible placing in different lines
        float getPixelWidth() const
        {
            float pixelWidth = 0;
            for (const auto& rFlowPart : mFlowParts)
            {
                pixelWidth += rFlowPart->getPixelWidth();
            }
            return pixelWidth;
        }

    protected:

        friend class TextFlow;

        // Members
        int mX; // relative in flow
        int mY; // relative in flow
        Type mType; // has to be set by creator!
        uint mContentStartIndex; // index in content where flow entity starts
        uint mIndex; // index within flow entity vector
        std::vector<std::unique_ptr<FlowPart> > mFlowParts;
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

        // Get flow entity, flow part index and letter index inside flow part. Maybe empty when index not available.
        // Content index of -1 indicates front of text.
        std::weak_ptr<const FlowEntity> getFlowEntityAndIndices(
            int contentIndex,
            uint& rFlowPartIndex,
            uint& rLetterIndex) const;

        // Insert content after index of exisiting content.
        // Returns flow entity and sets indices to position after insertion
        std::weak_ptr<const FlowEntity> insertContent(uint index, std::u16string content, uint& rFlowPartIndex, uint& rLetterIndex);

        // Erases letterCount many letters from content, beginning and including index and  excluding index + letterCount.
        // Negative index results in deletion of letters before index.
        // Returns flow entity and sets indices to position after deletion of content
        std::weak_ptr<const FlowEntity> eraseContent(int index, uint letterCount, uint& rFlowPartIndex, uint& rLetterIndex);

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
