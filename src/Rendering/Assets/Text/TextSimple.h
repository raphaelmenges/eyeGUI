//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Very simple text asset for rendering.
// TODO: Hebrew / right to left support

#ifndef TEXT_SIMPLE_H_
#define TEXT_SIMPLE_H_

#include "src/Rendering/Assets/Text/Text.h"

namespace eyegui
{
    class TextSimple : public Text
    {
    public:

        // Constructor
        TextSimple(
            GUI const * pGUI,
            AssetManager* pAssetManager,
            Font const * pFont,
            FontSize fontSize,
            float scale,
            std::u16string content);

		// Copy constructor
		TextSimple(const TextSimple& rOtherText);

        // Destructor
        virtual ~TextSimple();

        // Called by transform and size method of owner
        void transform();

		// Set position for drawing
		void setPosition(int x, int y);

        // Draw
        virtual void draw(
			glm::vec4 color,
			float alpha,
			bool renderBackground = false,
			int xOffset = 0,
			int yOffset = 0) const;

        // Getter
		int getX() const;
		int getY() const;
        int getTextWidth() const;
        int getTextHeight() const;
		std::u16string getContent() const;

    protected:

        // Calculate mesh (in pixel coordinates)
        virtual void specialCalculateMesh(
            std::u16string streamlinedContent,
            float lineHeight,
            RenderWordVertices& rVertices);

        // Members
        int mTextWidth;
        int mTextHeight;
	};
}

#endif // TEXT_SIMPLE_H_
