//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "IconElement.h"

#include "src/Layout.h"
#include "src/Utilities/OperationNotifier.h"

namespace eyegui
{
    IconElement::IconElement(
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
        std::string descKey) : InteractiveElement(
            id,
            styleName,
            pParent,
            pLayout,
            pFrame,
            pAssetManager,
            pNotificationQueue,
            renderingMask,
            relativeScale,
            border,
            dimming,
            adaptiveScaling)
    {
        mType = Type::ICON_ELEMENT;

        // Fill members
        setIcon(iconFilepath);
        mDescriptionKey = descKey;
        mDescriptionAlpha.setValue(0);

        // Create description (TODO: maybe merge somehow with text block init)
        std::u16string descriptionContent;
        if (mDescriptionKey != EMPTY_STRING_ATTRIBUTE)
        {
            std::u16string localization = mpLayout->getContentFromLocalization(mDescriptionKey);
            if (localization == LOCALIZATION_NOT_FOUND)
            {
                throwWarning(
                    OperationNotifier::Operation::RUNTIME,
                    "No localization used or one found for following key: " + mDescriptionKey + ". Element has following id: " + getId());

                descriptionContent = desc;
            }
            else
            {
                descriptionContent = localization;
            }
        }
        else
        {
            descriptionContent = desc;

        }

        // Check for non empty content. Otherwise, description flow unique pointer is just NULL
        if (!descriptionContent.empty())
        {
            mupDescriptionFlow = std::move(
                mpAssetManager->createTextFlow(
                    mpLayout->getDescriptionFontSize(),
                    TextFlowAlignment::CENTER,
                    TextFlowVerticalAlignment::CENTER,
                    1.f,
                    descriptionContent));
        }


    }

    IconElement::~IconElement()
    {
        // Nothing to do here
    }

    void IconElement::setIcon(std::string filepath)
    {
        if (filepath != EMPTY_STRING_ATTRIBUTE)
        {
            mpIcon = mpAssetManager->fetchTexture(filepath);
        }
        else
        {
            mpIcon = mpAssetManager->fetchTexture(graphics::Type::NOT_FOUND);
        }
    }

    void IconElement::setIcon(
		std::string name,
		int width,
		int height,
		ColorFormat format,
		unsigned char const * pIconData,
		bool flipY)
	{
        mpIcon = mpAssetManager->fetchTexture(name, width, height, format, pIconData, flipY);
    }

    float IconElement::specialUpdate(float tpf, Input* pInput)
    {
        // Super call
        float adaptiveScale = InteractiveElement::specialUpdate(tpf, pInput);

        // Check for penetration by input
        bool penetrated = penetratedByInput(pInput);

        // Make description visible
        if (mpLayout->getShowDescriptions() && penetrated)
        {
            mDescriptionAlpha.update(tpf / mpLayout->getConfig()->animationDuration);
        }
        else
        {
            mDescriptionAlpha.update(-tpf / mpLayout->getConfig()->animationDuration);
        }

        return adaptiveScale;
    }

    void IconElement::specialDraw() const
    {
        // Draw description
        if (mupDescriptionFlow != NULL && mDescriptionAlpha.getValue() > 0)
        {
            // Drawing of text flow
            mupDescriptionFlow->draw(getStyle()->fontColor, mDescriptionAlpha.getValue() * mAlpha, true);
        }

        // Draw stuff like highlighting
        InteractiveElement::specialDraw();
    }

    void IconElement::specialTransformAndSize()
    {
        // Tell description text flow about it
        if (mupDescriptionFlow != NULL)
        {
            mupDescriptionFlow->transformAndSize(mX, mY, mWidth, mHeight);
        }
    }

    void IconElement::specialReset()
    {
        InteractiveElement::specialReset();

        mDescriptionAlpha.setValue(0);
    }

    glm::vec2 IconElement::iconAspectRatioCorrection() const
    {
        float aspectRatio = (float)mWidth / (float)mHeight;
        float iconAspectRatio = mpIcon->getAspectRatio();
        float relation = aspectRatio / iconAspectRatio;
        glm::vec2 iconUVScale;
        if (relation >= 1)
        {
            // Render item wider than icon
            iconUVScale = glm::vec2(relation, 1.0f);
        }
        else
        {
            // Icon wider than render item
            iconUVScale = glm::vec2(1.0f, 1.0f / relation);
        }

        return iconUVScale;
    }
}
