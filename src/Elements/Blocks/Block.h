//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Block is a simple, uniform colored element.

#ifndef BLOCK_H_
#define BLOCK_H_

#include "Elements/Element.h"

namespace eyegui
{
    class Block : public Element
    {
    public:

        // Constructor
        Block(
            std::string id,
            std::string styleName,
            Element* pParent,
            Layout const * pLayout,
            Frame* pFrame,
            AssetManager* pAssetManager,
            NotificationQueue* pNotificationQueue,
            float relativeScale,
            float border,
            bool dimming,
            bool adaptiveScaling,
            bool consumeInput,
            std::string backgroundFilepath,
            ImageAlignment backgroundAlignment,
            float innerBorder = 0);

        // Destructor
        virtual ~Block();

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

        // Implemented by subclasses
        virtual bool mayConsumeInput();

        // Members
        int mInnerX;
        int mInnerY;
        int mInnerWidth;
        int mInnerHeight;

    private:

        // Members
        RenderItem const * mpBackground;
        std::unique_ptr<Image> mupImage;
        float mInnerBorder; // [0..1]
        bool mConsumeInput;
    };
}

#endif // BLOCK_H_
