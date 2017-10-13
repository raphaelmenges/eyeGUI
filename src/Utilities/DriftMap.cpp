//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "DriftMap.h"

#include <iostream>

namespace eyegui
{
	DriftMap::DriftMap()
	{
		
	}

	void DriftMap::notifyActivation(int gazeX, int gazeY, int centerX, int centerY, float activationDuration)
	{
		// TODO: activationDuration not used
		int driftX = gazeX - centerX;
		int driftY = gazeY - centerY;

		std::cout << "######################### HERE IS DRIFT MAP UPDATE" << std::endl;
		std::cout << "DriftX: " << driftX << " DriftY: " << driftY << std::endl;
	}

	void DriftMap::reset()
	{

	}
}