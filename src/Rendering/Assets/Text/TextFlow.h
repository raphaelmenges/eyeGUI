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
    class TextFlow : public Text
    {
    public:

		// Struct for sub flow word (if word is devided to fit into lines)
		struct SubFlowWord
		{
			int x; // relative in flow
			int y; // relative in flow
			int width; // pixel width
			// Height is given by line height

			// Vector which saves the x offsets of single letters inside sub word
			std::vector<int> lettersXOffsets; // position after letter
		};

		// Struct for word in flow which is used to get a word at given coordinate in text flow
		struct FlowWord
		{
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

		// Get data of certain word by index. Returns false if not found.
		// Position is given in flow coordinates
		bool getFlowWord(int index, FlowWord& rFlowWord) const;

		// Use position in flow coordinates to find a word. Returns false if not found
		bool getFlowWord(int x, int y, FlowWord& rFlowWord) const;

		// Add content after index of exisiting content
		void addContent(int index, std::u16string content);

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
