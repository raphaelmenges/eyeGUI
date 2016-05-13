//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Interactive element with single icon.

#ifndef ICON_ELEMENT_H_
#define ICON_ELEMENT_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"
#include "src/Rendering/Assets/Text/TextFlow.h"

namespace eyegui
{
    class IconElement : public InteractiveElement
    {
    public:

        // Constructors
        IconElement(
            std::string id,
            std::string styleName,
            Element* pParent,
            Layout const * pLayout,
            Frame* pFrame,
            AssetManager* pAssetManager,
            NotificationQueue* pNotificationQueue,
            RenderingMask renderingMask,
            float relativeScale,
            float border,
            bool dimming,
            bool adaptiveScaling,
            std::string iconFilepath,
            std::u16string desc,
            std::string descKey);

        // Destructor
        virtual ~IconElement() = 0;

        // Set icon
        void setIcon(std::string filepath);
        void setIcon(
			std::string name,
			int width,
			int height,
			ColorFormat format,
			unsigned char const * pIconData,
			bool flipY);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filled by subclasses
        virtual void specialReset();

        // Calculate aspect ratio correction for icon on gizmo
        glm::vec2 iconAspectRatioCorrection() const;

        // Members
        Texture const * mpIcon;
        std::string mDescriptionKey;
        std::unique_ptr<TextFlow> mupDescriptionFlow; // May be NULL when no description given
        LerpValue mDescriptionAlpha;


    };
}

#endif // ICON_ELEMENT_H_
