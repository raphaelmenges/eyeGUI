//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// TODO

#ifndef FUTURE_KEY_H_
#define FUTURE_KEY_H_

namespace eyegui
{
	// Forward declaration
	class Layout;
	class AssetManager;
	class RenderItem;

	// Class for key
	class FutureKey
	{
	public:

		// Constructor
		FutureKey(Layout const * pLayout, AssetManager* pAssetManager);

		// Destructor
		virtual ~FutureKey();

		// Simple transform and size to fit layout size
		virtual void transformAndSize(int x, int y, int width, int height);

		// Update
		virtual void update(float tpf);

		// Draw
		virtual void draw(float alpha) const;

		// Reset
		virtual void reset();

	private:

		// Members
		Layout const * mpLayout;
		AssetManager* mpAssetManager;

		int mX;
		int mY;
		int mWidth;
		int mHeight;
		RenderItem const * mpThresholdItem;
	};
}

#endif // FUTURE_KEY_H_