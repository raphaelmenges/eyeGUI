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
            Layout const * pLayout,
            Frame* pFrame,
            AssetManager* pAssetManager,
            NotificationQueue* pNotificationQueue,
            float relativeScale,
            float border,
            bool dimming,
            bool adaptiveScaling,
            std::string filepath,
            ImageAlignment alignment);

        // Destructor
        virtual ~Picture();

        // Check before transformation, how much space is needed
        virtual void evaluateSize(
            int availableWidth,
            int availableHeight,
            int& rWidth,
            int& rHeight) const;

        // Set image in picture
		void setContentOfImage(
			std::string name,
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pData,
			bool flipY);

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

    private:

        // Members
        std::unique_ptr<Image> mupImage;
    };
}

#endif // PICTURE_H_
