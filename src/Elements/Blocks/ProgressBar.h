//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Progress bar to visualize any progress with ease.

#ifndef PROGRESS_BAR_H_
#define PROGRESS_BAR_H_

#include "Elements/Blocks/Block.h"

namespace eyegui
{

    // Directions of progress visualization
   

    class ProgressBar : public Block
    {
    public:

		enum class Direction
		{
			LEFT_TO_RIGHT, RIGHT_TO_LEFT, TOP_TO_BOTTOM, BOTTOM_TO_TOP
		};

        // Destructor
        virtual ~ProgressBar();

        // Set progress. Clamped between 0 and 1
        void setProgress(float progress);

    protected:

		// Factory is friend
		friend ElementFactory;

		// Protected constructor
		ProgressBar(
			std::string id,
			std::vector<std::string> styles,
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
			float innerBorder,
			Direction progressDirection);

        // Drawing filled by subclasses
        virtual void specialDraw() const;

    private:

        // Members
        Direction mDirection;
        float mProgress; // [0..1]
        RenderItem const * mpProgressItem;
    };
}

#endif // PROGRESS_BAR_H_
