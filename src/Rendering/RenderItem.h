//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Encapsulation of the OpenGL vertex array object creation and usage. Forms
// pair of shader and mesh and binds shader and vertex array object at once.

#ifndef RENDER_ITEM_H_
#define RENDER_ITEM_H_

#include "Shader.h"
#include "Mesh.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    class RenderItem
    {
    public:

        // Constructor
        RenderItem(Shader const * pShader, Mesh const * pMesh);

        // Destructor
        virtual ~RenderItem();

        // Bind whole render item (must used before filling values in shader and drawing)
        void bind() const;

        // Draw
        void draw(GLenum mode = GL_TRIANGLES) const;

        // Get pointer to shader
        Shader const * getShader() const;

        // Get pointer to mesh
        Mesh const * getMesh() const;

    private:

        // Member
        Shader const * mpShader;
        Mesh const * mpMesh;
        GLuint mVertexArrayObject;
    };
}

#endif // RENDER_ITEM_H_
