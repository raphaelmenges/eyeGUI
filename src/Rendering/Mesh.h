//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Encapsulation of the OpenGL mesh creation and usage.

#ifndef MESH_H_
#define MESH_H_

#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <vector>

namespace eyegui
{
	class Mesh
	{
	public:

		// Constructor
		Mesh(std::vector<float> const * const pVertices, std::vector<float> const * const pTextureCoordinates);

		// Deconstructor
		virtual ~Mesh();

		// Getter
		GLuint getVertexCount() const;
		GLuint getVertexBuffer() const;
		GLuint getTextureCoordinateBuffer() const;

	private:

		// Members
		GLuint mVertexCount;
		GLuint mVertexBuffer;
		GLuint mTextureCoordinateBuffer;
	};
}

#endif // MESH_H_
