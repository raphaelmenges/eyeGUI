//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_SUGGESTION_H_
#define FUTURE_SUGGESTION_H_

#include "include/eyeGUI.h"
#include "src/Utilities/LerpValue.h"

#include <memory>

namespace eyegui
{
	// Forward declaration
	class Layout;
	class AssetManager;
	class RenderItem;
    class TextSimple;
    struct Input;

	// Class for suggestion
	class FutureSuggestion
	{
	public:

		// Constructor
		FutureSuggestion(
            Layout const * pLayout,
            AssetManager* pAssetManager,
            float fontScale);

		// Destructor
		virtual ~FutureSuggestion();

		// Simple transform and size to fit layout size
		virtual void transformAndSize(int x, int y, int width, int height);

        // Update, returns whether hit
        virtual bool update(float tpf, Input const * pInput);

		// Draw
        virtual void draw(
			glm::vec4 backgroundColor,
            glm::vec4 fontColor,
            glm::vec4 thresholdColor,
            float alpha) const;

		// Reset
		virtual void reset();

        // Get suggestion
        std::u16string getSuggestion();

        // Set suggestion
        void setSuggestion(std::u16string suggestion);

		// Clear suggestion
		void clearSuggestion();
		
	private:

        // Update suggestion position
        void transformSuggestion();

        // Constants
        const float PRESS_DURATION = 0.5f;
        const float RETRIGGER_DELAY = 0.5f;
		const float SUGGESTION_ANIMATION_DURATION = 0.5f;

		// Members
		Layout const * mpLayout;
		AssetManager* mpAssetManager;

		int mX;
		int mY;
		int mWidth;
		int mHeight;
        float mFontScale;
        RenderItem const * mpBackgroundItem;
		RenderItem const * mpThresholdItem;
        std::unique_ptr<TextSimple> mupSuggestion;
        LerpValue mThreshold;
        float mRetriggerTime;
        LerpValue mPressing;
        std::pair<float, std::unique_ptr<TextSimple> > mSuggestionAnimation;
	};
}

#endif // FUTURE_KEY_H_
