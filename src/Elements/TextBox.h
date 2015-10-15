//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO:
//  - style color "font-color" or so

#ifndef TEXT_BOX_H_
#define TEXT_BOX_H_

#include "Element.h"
#include "Rendering/Font/TextFlow.h"

namespace eyegui
{
    class TextBox : public Element
    {
    public:

        // Constructors
        TextBox(
            std::string id,
            std::string styleName,
            Element* pParent,
            Layout* pLayout,
            Frame * pFrame,
            AssetManager* pAssetManager,
            float relativeScale,
            float border,
            FontSize fontSize,
            TextFlowAlignment alignment,
            std::u16string content,
            float innerBorder);

        // Destructor
        virtual ~Picture();

        // Check before transformation, how much space is needed
        virtual void evaluateSize(
            int availableWidth,
            int availableHeight,
            int& rWidth,
            int& rHeight) const;

    protected:

        // Updating filled by subclasses
        virtual void specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

    private:

        // Members
        RenderItem const * mpQuad;

        PictureAlignment mAlignment;
    };
}

#endif // TEXT_BOX_H_
