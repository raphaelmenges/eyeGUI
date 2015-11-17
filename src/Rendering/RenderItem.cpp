//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "RenderItem.h"

namespace eyegui
{
    RenderItem::RenderItem(Shader const * pShader, Mesh const * pMesh)
    {
        // Fill members
        mpShader = pShader;
        mpMesh = pMesh;

        // Save currently set buffer and vertex array object
        GLint oldBuffer, oldVAO;
        glGetIntegerv(GL_ARRAY_BUFFER, &oldBuffer);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &oldVAO);

        // Vertex array object
        mVertexArrayObject = 0;
        glGenVertexArrays(1, &mVertexArrayObject);
        glBindVertexArray(mVertexArrayObject);

        // Vertices
        GLuint vertexAttrib = glGetAttribLocation(mpShader->getShaderProgram(), "posAttribute");
        glEnableVertexAttribArray(vertexAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mpMesh->getVertexBuffer());
        glVertexAttribPointer(vertexAttrib, 3, GL_FLOAT, GL_FALSE, 0, NULL);

        // Texture coordinates
        GLuint uvAttrib = glGetAttribLocation(mpShader->getShaderProgram(), "uvAttribute");
        glEnableVertexAttribArray(uvAttrib);
        glBindBuffer(GL_ARRAY_BUFFER, mpMesh->getTextureCoordinateBuffer());
        glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, 0, NULL);

        // Restore old settings
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
        glBindVertexArray(oldVAO);
    }

    RenderItem::~RenderItem()
    {
        glDeleteVertexArrays(1, &mVertexArrayObject);
    }

    void RenderItem::bind() const
    {
        mpShader->bind();
        glBindVertexArray(mVertexArrayObject);
    }

    void RenderItem::draw(GLenum mode) const
    {
        glDrawArrays(mode, 0, mpMesh->getVertexCount());
    }

    Shader const * RenderItem::getShader() const
    {
        return mpShader;
    }

    Mesh const * RenderItem::getMesh() const
    {
        return mpMesh;
    }
}
