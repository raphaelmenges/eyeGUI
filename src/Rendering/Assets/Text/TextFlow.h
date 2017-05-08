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
            if(mupRenderWord != nullptr)
            {
                return mupRenderWord->getLetterCount();
            }
            else
            {
                return 1;
            }
        }

        // Getter for pixel width
        float getPixelWidth() const
        {
            if(mupRenderWord != nullptr)
            {
                return mupRenderWord->pixelWidth;
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
		int getXOffset(int offsetIndex) const
		{
			// Increment offset index to fit render words implementation
			offsetIndex++;

			if (mupRenderWord != nullptr)
			{
				if (offsetIndex < (int)mupRenderWord->xOffsets.size()) // index found
				{
					return mupRenderWord->xOffsets.at(offsetIndex);
				}
				else // not found
				{
					return 0;
				}
				
			}
			else // no render word
			{
				// For space: Since each space is represented by one flow part, return pixel width when index is 1
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

		// TextFlow fills values
        friend class TextFlow;

		// Constructor
		FlowPart() {}

        // Members
        int mX = 0; // relative in flow
        int mY = 0; // relative in flow
        bool mCollapsed = false;
        std::unique_ptr<RenderWord> mupRenderWord = nullptr; // geometry information of word
        float mPixelWidth = 0; // fallback when mupWord is nullptr
    };

    // Class for flow entities. Consists at least of one flow part
    class FlowEntity
    {
    public:

        // Enumeration of types
        enum class Type { Word, Space, Mark, NewLine };

        // Getter for type
        Type getType() const { return mType; }

		// Getter for index where represented string starts in content of text flow
        uint getContentStartIndex() const { return mContentStartIndex; }

        // Getter for index of flow entity within entities vector of text flow
        uint getIndex() const { return mIndex; }

		// Check whether flow entity represents whitespace
		bool isWhitespace() const
		{
			return mType == Type::Space || mType == Type::NewLine;
		}

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

		// Calculates index of flow part and letter within flow part of given letter within the represented string.
		// -1 means in front of the string, everything else within. Returns whether successful
		bool getIndices(int stringLetterIndex, uint& rFlowPartIndex, int& rLetterIndex) const
		{
			// Case when in front of string
			if (stringLetterIndex == -1)
			{
				rFlowPartIndex = 0;
				rLetterIndex = -1;
				return true;
			}
			else if(stringLetterIndex >= 0)
			{
				int localIndex = (int)stringLetterIndex;

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
		
		// Return direction of rendered text
		bool isRightToLeft() const
		{
			return rightToLeft;
		}

    protected:

		// TextFlow fills values
        friend class TextFlow;

		// Constructor
		FlowEntity() {}

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

		// Getter for position within text flow
		int getX() const { return mX; }
		int getY() const { return mY; }

        // Members
        int mX = 0; // relative in flow
        int mY = 0; // relative in flow
        Type mType; // set by text flow
        uint mContentStartIndex = 0; // index in content where flow entity starts
        uint mIndex = 0; // index within flow entity vector
        std::vector<std::shared_ptr<FlowPart> > mFlowParts;
		bool rightToLeft = false; // bool indicating rendering direction
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

		// Get content at given index plus length. Includes index and excludes index + letterCount
        std::u16string getContent(uint contentIndex, uint letterCount) const;

        // Get weak pointer to flow entity at index. May be empty when index not available
        std::weak_ptr<const FlowEntity> getFlowEntity(uint entityIndex) const;

        // Use position in flow coordinates to get an entity. May be empty when index not available.
        // Coordinates relative in pixel space of flow
        std::weak_ptr<const FlowEntity> getFlowEntity(int x, int y) const;

        // Get flow entity, flow part index and letter index inside flow part. May be empty when index not available.
        // Content index of -1 indicates front of text.
        std::weak_ptr<const FlowEntity> getFlowEntityAndIndices(
            int contentIndex,
            uint& rFlowPartIndex,
            int& rLetterIndex) const;

        // Insert content after index of exisiting content.
        // Returns flow entity and sets indices to position after insertion
        std::weak_ptr<const FlowEntity> insertContent(int contentIndex, std::u16string content, uint& rFlowPartIndex, int& rLetterIndex);

        // Erases letterCount many letters from content, beginning and including index and excluding index + letterCount.
        // Negative letter count results in deletion of letters before index.
        // Returns flow entity and sets indices to front position of deletion
        std::weak_ptr<const FlowEntity> eraseContent(int contentIndex, int letterCount, uint& rFlowPartIndex, int& rLetterIndex);

    protected:

        // Calculate mesh (in pixel coordinates)
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight,
            RenderWordVertices& rVertices);

        // Calculate word with maximal width (in doubt split it). If result is empty, not enough space available
        std::vector<RenderWord> calculateFitWord(std::u16string content, int maxPixelWidth, float scale, bool rightToLeft) const;

        // Inserts word into render words vector, returns true at success and false if not enough space available
        bool insertFitWord(std::vector<RenderWord>& rWords, const std::u16string& rContent, int maxPixelWidth, float scale, bool rightToLeft) const;

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
