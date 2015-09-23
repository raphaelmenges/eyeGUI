//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Picture element displaying vector or pixel based images.

#ifndef PICTURE_H_
#define PICTURE_H_

#include "Element.h"

namespace eyegui
{
    class Picture : public Element
    {
    public:

        // Constructors
        Picture(
            std::string id,
            std::string styleName,
            Element* pParent,
            Layout* pLayout,
            Frame * pFrame,
            AssetManager* pAssetManager,
            float relativeScale,
            float border,
            std::string filepath,
            PictureAlignment alignment);

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
        RenderItem* mpQuad;
        Texture* mpImage;
        PictureAlignment mAlignment;
    };
}

#endif // PICTURE_H_
