//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Mesh.h"

namespace eyegui
{
    Mesh::Mesh(std::vector<float> const * const pVertices, std::vector<float> const * const pTextureCoordinates)
    {
        // Save currently set buffer
        GLint oldBuffer = -1;
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &oldBuffer);

        // Vertex buffer
        mVertexBuffer = 0;
        glGenBuffers(1, &mVertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mVertexBuffer);
        glBufferData(GL_ARRAY_BUFFER, pVertices->size() * sizeof(float), pVertices->data(), GL_STATIC_DRAW);

        // Texture coordinate buffer
        mTextureCoordinateBuffer = 0;
        glGenBuffers(1, &mTextureCoordinateBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, mTextureCoordinateBuffer);
        glBufferData(GL_ARRAY_BUFFER, pTextureCoordinates->size() * sizeof(float), pTextureCoordinates->data(), GL_STATIC_DRAW);

        // Save vertex count
        mVertexCount = static_cast<GLuint>(pVertices->size()) / 3;

        // Restore old setting
        glBindBuffer(GL_ARRAY_BUFFER, oldBuffer);
    }

    Mesh::~Mesh()
    {
        // Delete buffers
        glDeleteBuffers(1, &mVertexBuffer);
        glDeleteBuffers(1, &mTextureCoordinateBuffer);
    }

    GLuint Mesh::getVertexCount() const
    {
        return mVertexCount;
    }

    GLuint Mesh::getVertexBuffer() const
    {
        return mVertexBuffer;
    }

    GLuint Mesh::getTextureCoordinateBuffer() const
    {
        return mTextureCoordinateBuffer;
    }
}
