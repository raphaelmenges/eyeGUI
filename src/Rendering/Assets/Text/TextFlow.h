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

        // Getter for index within entities vector
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

        // Getter for flow part and letter index for given letter offset. Returns whether successful.
        // Letter index is index inside of flow part
        bool getIndices(uint inputWordIndex, uint& rFlowPartIndex, uint& rLetterIndex) const
        {
            // Only continue when enough letters are available
            if(inputWordIndex < getLetterCount())
            {
                // TODO: if bug occurs, it may happen here since letterXOffset has one element more than before
                int offset = inputWordIndex;
                for(uint i = 0; i < getFlowPartCount(); i++)
                {
                    if((offset - mFlowParts.at(i)->getLetterCount()) < 0)
                    {
                        rFlowPartIndex = i;
                        rLetterIndex = offset;
                        return true;
                    }
                    else
                    {
                        offset -= mFlowParts.at(i)->getLetterCount();
                    }
                }
            }
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
            index += letterIndex;
            index += mContentStartIndex;
            return index;
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

        // Get flow entity, flow part index and letter index inside flow part. Maybe empty when index not available.
        // Content index of -1 indicates front of text
        std::weak_ptr<const FlowEntity> getFlowEntityAndIndices(
            uint contentIndex,
            uint& rFlowPartIndex,
            uint& rLetterIndex) const;






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
