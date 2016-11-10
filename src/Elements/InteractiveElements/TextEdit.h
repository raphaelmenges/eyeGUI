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

        // Destructor
        virtual ~TextEdit();

		// Move cursor given count of letters. If content is exceeded, last valid position is set. Spaces are counted as one letter.
		// Negative letter count indicates leftward movement, positive rightward movement
		void moveCursorOverLetters(int letterCount);

		// Move cursor given count of words. If content is exceeded, last valid position is set. Sets cursor always behind word
		// Negative word count indicates leftward movement, positive rightward movement
		void moveCursorOverWords(int wordCount);

        // Move cursor to start of text
        void moveCursorToStart();

        // Move cursor to end of text
        void moveCursorToEnd();

		// Add content behind cursor position
		void addContentAtCursor(std::u16string content);

		// Set complete content and set cursor after end of content
		void setContent(std::u16string content);

		// Delete content at cursor. Positive value deletes rightward, negative leftward
		void deleteContentAtCursor(int letterCount);

		// Getter for active word. Returns empty string if there is no
		std::u16string getActiveWord() const;

    protected:

		// Factory is friend
		friend ElementFactory;

		// Protected constructor
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

		// Calculate text flow y offset in pixels
		int calculateTextFlowYOffset() const;

		// Set new active word
        void setActiveWord(const FlowEntity& rFlowEntity, bool setCursorToEnd);

        // Move cursor over letter rightward
        void moveCursorOverLettersRightward(int letterCount);

        // Move cursor over letter leftward
        void moveCursorOverLettersLeftward(int letterCount);

		// Typedef
        //typedef std::pair<std::vector<SubFlowWord>, float> SubFlowWordAlphaPair;

        // Members
        RenderItem const * mpBackground;
		RenderItem const * mpCursor;
		RenderItem const * mpActiveWordBackground;

		// Text flow
        std::unique_ptr<TextFlow> mupTextFlow;
		LerpValue mTextFlowYOffset; // as relative value [0..1]
		FontSize mFontSize;

		// Active word and cursor position
        //std::unique_ptr<FlowWord> mupActiveWord; // unique pointer to active flow word. Defines indirectly position of cursor, which is somewhere inside that word
		int mSubWordIndex; // inside active sub word
		int mLetterIndex; // inside active sub word's letters. Minus one indicates, that cursor is at beginning of word
		
		// Animation related members
		float mCursorPulse; // [0..2*Pi]
		float mActiveWordFading; // [0..AnimationDuration]
        //std::vector<SubFlowWordAlphaPair> mPreviousActiveWords; // float is initialized with animation
																								 // duration and decremented at each update
    };
}

#endif // TEXT_EDIT_H_
