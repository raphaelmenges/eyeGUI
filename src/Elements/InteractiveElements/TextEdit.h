//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Element for text editing. Delegates rendering to text flow asset. Saves
// a pointer to the active entity and highlights it for the user. Within the
// active entity, a cursor position is set, which is used for deleting and
// inserting new content to the text that is edited.

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

		// Get complete content
		std::u16string getContent() const;

		// Delete content at cursor. Positive value deletes rightward, negative leftward
		void deleteContentAtCursor(int letterCount);

		// Getter for content in active entity. Returns empty string if there is no
		std::u16string getActiveEntityContent() const;

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

		// Set new active entity
        void setActiveEntity(std::weak_ptr<const FlowEntity> wpFlowEntity, bool setCursorToEnd);

        // Move cursor over letter rightward
        void moveCursorOverLettersRightward(int letterCount);

        // Move cursor over letter leftward
        void moveCursorOverLettersLeftward(int letterCount);

        // Members
        RenderItem const * mpBackground;
		RenderItem const * mpCursor;
		RenderItem const * mpActiveEntityBackground;

		// Text flow
        std::unique_ptr<TextFlow> mupTextFlow;
		LerpValue mTextFlowYOffset; // as relative value [0..1]
		FontSize mFontSize;

		// Active entity
		std::weak_ptr<const FlowEntity> mwpActiveEntity; // weak pointer to active flow entity. Defines indirectly position of cursor, which is somewhere inside that entitiy
		
		// CursorPosition
		uint mCursorFlowPartIndex; // flow part inside active entity
		int mCursorLetterIndex; // inside active entity's flow part. -1 when in front of flow part
		
		// Animation related members
		float mCursorPulse; // [0..2*Pi]
		float mActiveEntityFading; // [0..AnimationDuration]
    };
}

#endif // TEXT_EDIT_H_
