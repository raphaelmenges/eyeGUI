//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Visualizes gaze.

#ifndef GAZE_DRAWER_H_
#define GAZE_DRAWER_H_

#include "AssetManager.h"
#include "externals/GLM/glm/glm.hpp"

#include <vector>

namespace eyegui
{
	class GazeDrawer
	{
	public:

		// Constructor
		GazeDrawer(); // TODO: asset manager (create line asset ?!)

		// Destructor
		virtual ~GazeDrawer();

		// Update
		void update(int gazeX, int gazeY, float tpf);

		// Draw
		void draw() const;

	private:

		// Structure for gaze points
		struct GazePoint
		{
			glm::vec2 point; // Easier to handle than ivec2
			float alpha = 1;
			float size = 1;
		};

		// Members
		std::vector<GazePoint> points;
	};
}

#endif // GAZE_DRAWER