//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_KEY_H_
#define FUTURE_KEY_H_

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
    class Input;

	// Class for key
	class FutureKey
	{
	public:

        // Enumeration of values which are returned
        enum class HitType { NONE, LETTER, SUGGESTION };

		// Constructor
        FutureKey(
            std::string id,
            Layout const * pLayout,
            AssetManager* pAssetManager,
            std::u16string letter,
            float letterScale,
            bool showSuggestion,
            KeyboardCase keyCase = KeyboardCase::UPPER,
            bool ignoreCase = false);

		// Destructor
		virtual ~FutureKey();

		// Simple transform and size to fit layout size
		virtual void transformAndSize(int x, int y, int width, int height);

        // Update, returns whether hit
        virtual HitType update(float tpf, Input const * pInput);

		// Draw
		virtual void draw(float alpha) const;

		// Reset
		virtual void reset();

        // Get id
        std::string getId() const { return mId; }

        // Set case
        void setCase(KeyboardCase keyCase);

        // Toggle case
        void toggleCase() { setCase((mKeyCase == KeyboardCase::LOWER) ? KeyboardCase::UPPER : KeyboardCase::LOWER); }

        // Get letter
        std::u16string getLetter();

        // Get suggestion
        std::u16string getSuggestion();

	private:

        // Update letter position
        void transformLetter();

        // Update suggestion position
        void transformSuggestion();

        // Constants
        const float SUGGESTION_HEIGHT = 0.2f;
        const float LETTER_FADING_MULTIPLIER = 0.7f;
        const float LETTER_Y_OFFSET_MULTIPLIER = 0.125f;
        const int BACKGROUND_PIXEL_BULGE = 1;
        const float PRESS_DURATION = 0.5f;
        const float SUGGESTION_ANIMATION_DURATION = 0.5f;
        const float RETRIGGER_DELAY = 1.f;

		// Members
		Layout const * mpLayout;
		AssetManager* mpAssetManager;

        std::string mId;
		int mX;
		int mY;
		int mWidth;
		int mHeight;
        float mLetterScale;
        bool mShowSuggestion;
        KeyboardCase mKeyCase;
        bool mIgnoreCase;
        RenderItem const * mpKeyItem;
        RenderItem const * mpBackgroundItem; // that' REALLY hacky because background is bigger then elements extend
        RenderItem const * mpSuggestionBackgroundItem;
		RenderItem const * mpThresholdItem;
        glm::mat4 mSuggestionBackgroundDrawMatrix;
        glm::mat4 mThresholdDrawMatrix;
        std::unique_ptr<TextSimple> mupLetter;
        std::unique_ptr<TextSimple> mupSuggestion;
        LerpValue mFirstThreshold;
        LerpValue mSecondThreshold;
        float mRetriggerTime;
        bool mDoingSecondThreshold;
        LerpValue mLetterFading;
        LerpValue mPressing;
        std::pair<float, std::unique_ptr<TextSimple> > mSuggestionAnimation;
	};
}

#endif // FUTURE_KEY_H_
