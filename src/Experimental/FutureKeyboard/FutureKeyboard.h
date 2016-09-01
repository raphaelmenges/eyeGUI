//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_KEYBOARD_H_
#define FUTURE_KEYBOARD_H_

#include "src/Elements/InteractiveElements/InteractiveElement.h"
#include "src/Experimental/FutureKeyboard/FutureKey.h"
#include "src/Experimental/FutureKeyboard/FutureSuggestion.h"

namespace eyegui
{
    class FutureKeyboard : public InteractiveElement, public NotifierTemplate<eyegui_experimental::FutureKeyboardListener>
	{
	public:

        // Modes
        enum class Mode { ONE_SUGGESTION_LINE, MANY_SUGGESTION_LINES, SUGGESTION_PER_KEY };

		// Constructor
		FutureKeyboard(
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
			Mode mode);

		// Destructor
		virtual ~FutureKeyboard();

		// Setter for suggestions in line
		void setSuggestionLine(std::u16string suggestionA, std::u16string suggestionB, std::u16string suggestionC);

        // Setter for suggestion on key
        void setKeySuggestion(std::string keyId, std::u16string suggestion);

		// Get content
		std::u16string getContent() const;

		// Set next sentence
		void nextSentence(std::u16string sentence);

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

        // Update display
        void updateDisplay();

		// Update suggestions
		void updateSuggestions();

		// Build content
		std::u16string buildContent() const;

		// Members
        Mode mMode;
        std::u16string mLastLetter;
		int mLastLine;
		bool mLastWasSuggestion;
		std::u16string mWordBeforeSuggestion;
		std::u16string mCollectedWordsBeforeSuggestion;

		// List of all suggestions
		std::vector<std::shared_ptr<FutureSuggestion> > mSuggestionList;

		// Suggestions in suggestion line
		std::shared_ptr<FutureSuggestion> mspSuggestionA;
		std::shared_ptr<FutureSuggestion> mspSuggestionB;
		std::shared_ptr<FutureSuggestion> mspSuggestionC;
		
		// Placeholder for lines without suggestions
		RenderItem const * mpEmptySuggestion;
		glm::mat4 mEmptySuggestionADrawMatrix;
		glm::mat4 mEmptySuggestionBDrawMatrix;
		glm::mat4 mEmptySuggestionCDrawMatrix;
		glm::mat4 mEmptySuggestionDDrawMatrix;
		glm::mat4 mEmptySuggestionEDrawMatrix;
		glm::mat4 mEmptySuggestionFDrawMatrix;

        // List of all keys
        std::vector<std::shared_ptr<FutureKey> > mKeyList;

        // First row
        std::shared_ptr<FutureKey> mspQKey;
        std::shared_ptr<FutureKey> mspWKey;
        std::shared_ptr<FutureKey> mspEKey;
        std::shared_ptr<FutureKey> mspRKey;
        std::shared_ptr<FutureKey> mspTKey;
        std::shared_ptr<FutureKey> mspYKey;
        std::shared_ptr<FutureKey> mspUKey;
        std::shared_ptr<FutureKey> mspIKey;
        std::shared_ptr<FutureKey> mspOKey;
        std::shared_ptr<FutureKey> mspPKey;

        // Second row
        std::shared_ptr<FutureKey> mspAKey;
        std::shared_ptr<FutureKey> mspSKey;
        std::shared_ptr<FutureKey> mspDKey;
        std::shared_ptr<FutureKey> mspFKey;
        std::shared_ptr<FutureKey> mspGKey;
        std::shared_ptr<FutureKey> mspHKey;
        std::shared_ptr<FutureKey> mspJKey;
        std::shared_ptr<FutureKey> mspKKey;
        std::shared_ptr<FutureKey> mspLKey;
        std::shared_ptr<FutureKey> mspEnterKey;

        // Third row
        std::shared_ptr<FutureKey> mspZKey;
        std::shared_ptr<FutureKey> mspXKey;
        std::shared_ptr<FutureKey> mspCKey;
        std::shared_ptr<FutureKey> mspVKey;
        std::shared_ptr<FutureKey> mspBKey;
        std::shared_ptr<FutureKey> mspNKey;
        std::shared_ptr<FutureKey> mspMKey;
        std::shared_ptr<FutureKey> mspBackspaceKey;

        // Forth row
        std::shared_ptr<FutureKey> mspShiftKey;
        std::shared_ptr<FutureKey> mspRepeatKey;
        std::shared_ptr<FutureKey> mspCommaKey;
        std::shared_ptr<FutureKey> mspSpaceKey;
        std::shared_ptr<FutureKey> mspDotKey;
        std::shared_ptr<FutureKey> mspQuestionKey;
        std::shared_ptr<FutureKey> mspExclamationKey;
        std::shared_ptr<FutureKey> mspColonKey;

        // Display for text
        std::u16string mCurrentWord; // current word, no spaces included
        std::u16string mCollectedWords; // collected words, included spaces
        std::unique_ptr<TextFlow> mupDisplay;
        std::unique_ptr<TextFlow> mupPreDisplay;
	};
}

#endif // FUTURE_KEYBOARD_H_
