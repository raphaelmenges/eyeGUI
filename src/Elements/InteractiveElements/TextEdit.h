//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef TEXT_EDIT_H_
#define TEXT_EDIT_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"

namespace eyegui
{
    class TextEdit : public InteractiveElement, public NotifierTemplate<TextEdit>
    {
    public:

        // Constructor
		TextEdit(
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
            FontSize fontSize);

        // Destructor
        virtual ~TextEdit();

		// Move cursor given count of letters. If content is exceeded, last valid position is set. Spaces are counted as one letter.
		// Negative letter count indicates leftward movement, positive rightward movement
		void moveCursorOverLetters(int letterCount);

		// Move cursor given count of words. If content is exceeded, last valid position is set. Sets cursor always behind word
		// Negative word count indicates leftward movement, positive rightward movement
		void moveCursorOverWords(int wordCount);

    protected:

        // Updating filled by subclasses, returns adaptive scale
        virtual float specialUpdate(float tpf, Input* pInput);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

        // Transformation filled by subclasses
        virtual void specialTransformAndSize();

        // Reset filld by subclasses
        virtual void specialReset();

        // Interaction fill by subclasses
        virtual void specialInteract();

        // Filled by subclass and called by layout after updating and before drawing
        virtual void specialPipeNotification(NotificationType notification, Layout* pLayout);

    private:

		// Calculate text flow y offset
		int calculateTextFlowYOffset() const;

		// Set new active word
		void setActiveWord(const std::vector<TextFlow::SubFlowWord>& rSubFlowWords, bool setCursorToEnd);

		// Typedef
		typedef std::pair<std::vector<TextFlow::SubFlowWord>, float> SubFlowWordAlphaPair;

        // Members
        RenderItem const * mpBackground;
		RenderItem const * mpCursor;
		RenderItem const * mpActiveWordBackground;

		// Text flow
		std::unique_ptr<TextFlow> mupTextFlow;
		LerpValue mTextFlowYOffset;
		FontSize mFontSize;

		// Active word and cursor
		std::unique_ptr<SubFlowWordAlphaPair> mupActiveWord; // unique pointer to active word
		int mCursorSubFlowWordIndex; // inside active sub word
		int mCursorSubFlowLetterIndex; // inside active sub word's letters
		
		// Animation related members
		float mCursorPulse; // [0..2*Pi]
		std::vector<SubFlowWordAlphaPair> mPreviousActiveWords; // float is initialized with animation
																								 // duration and decremented at each update
    };
}

#endif // TEXT_EDIT_H_
