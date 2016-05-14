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

        // Constructor
        TextFlow(
            GUI const * pGUI,
            AssetManager* pAssetManager,
            Font const * pFont,
            FontSize fontSize,
            TextFlowAlignment alignment,
            TextFlowVerticalAlignment verticalAlignment,
            float scale,
            std::u16string content);

        // Destructor
        virtual ~TextFlow();

        // Transform and size (has to be called before first usage)
        void transformAndSize(
            int x,
            int y,
            int width,
            int height);

        // Draw
        virtual void draw(glm::vec4 color, float alpha, bool renderBackground = false) const;

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
    };
}

#endif // TEXT_FLOW_H_
