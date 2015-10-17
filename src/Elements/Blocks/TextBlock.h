//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef TEXT_BLOCK_H_
#define TEXT_BLOCK_H_

#include "Block.h"
#include "Rendering/Font/TextFlow.h"

namespace eyegui
{
    class TextBlock : public Block
    {
    public:

        // Constructors
		TextBlock(
			std::string id,
			std::string styleName,
			Element* pParent,
			Layout* pLayout,
			Frame* pFrame,
			AssetManager* pAssetManager,
			float relativeScale,
			float border,
			FontSize fontSize,
			TextFlowAlignment alignment,
			TextFlowVerticalAlignment verticalAlignment,
			std::u16string content,
			std::string key,
            float innerBorder);

        // Destructor
        virtual ~TextBlock();

    protected:

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation
        virtual void specialTransformAndSize();

    private:

        // Members
		std::string mKey;
        float mInnerBorder;
        std::unique_ptr<TextFlow> mupTextFlow;
    };
}

#endif // TEXT_BLOCK_H_
