//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Specialization of texture for vector based images.

#ifndef VECTOR_TEXTURE_H_
#define VECTOR_TEXTURE_H_

#include "Texture.h"

#include "externals/NanoSVG/nanosvg.h"

namespace eyegui
{
	class VectorTexture : public Texture
	{
	public:

		// Constructor for graphics on disk
		VectorTexture(std::string filepath, Filtering filtering, Wrap wrap);

		// Constructor for internal graphics
		VectorTexture(std::string const * pGraphic, Filtering filtering, Wrap wrap);

		// Destructor
		virtual ~VectorTexture();

	private:

		// Rasterize
		void rasterizeGraphics(NSVGimage* svg, Filtering filtering, Wrap wrap);

	};
}

#endif // VECTOR_TEXTURE_H_
