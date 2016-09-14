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
#include "src/Rendering/Assets/Text/FlowEntity.h"

namespace eyegui
{
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

		// Get height (interesting if overflowHeight is true)
		int getHeight() const { return mHeight; }

		// Get pixel width of space letter in used font
		float getPixelWidthOfSpace() const { return mPixelOfSpace; }

        // Get count of flow words
        int getFlowWordCount() const { return (int)mFlowEntities.size(); }

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

		// Get content at given index plus length
		std::u16string getContent(int index, int letterCount) const;

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

        // Members
        TextFlowAlignment mAlignment;
        TextFlowVerticalAlignment mVerticalAlignment;
        int mWidth;
        int mHeight;
		int mFlowWidth;
        int mFlowHeight;
		float mPixelOfSpace;
		bool mOverflowHeight; // when overflow height, height in transformAndSize is ignored and overwritten by height necessary to display complete text
		std::vector<std::unique_ptr<FlowEntity> > mFlowEntities; // holding information to make text manipulation possible
	};
}

#endif // TEXT_FLOW_H_
