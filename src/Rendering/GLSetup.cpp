//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GLSetup.h"

namespace eyegui
{
    void GLSetup::setup(int viewportX, int viewportY, int viewportWidth, int viewportHeight)
    {
        // Storing
        glGetIntegerv(GL_VIEWPORT, mViewport);
        glGetBooleanv(GL_BLEND, &mBlend);
        glGetBooleanv(GL_DEPTH_WRITEMASK, &mDepthMask);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &mBlendSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &mBlendDst);
        glGetIntegerv(GL_CURRENT_PROGRAM, &mShaderProgram);
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &mVertexArrayObject);
        glGetBooleanv(GL_CULL_FACE, &mCulling);
        glGetBooleanv(GL_COLOR_WRITEMASK, mColorWritemask);
        glGetBooleanv(GL_STENCIL_TEST, &mStencilTest);
		glGetBooleanv(GL_SCISSOR_TEST, &mScissorTest);
		glGetIntegerv(GL_SCISSOR_BOX, mScissorBox);
		glGetIntegerv(GL_ACTIVE_TEXTURE, &mTextureSlot);

        // Setting
        glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
        glEnable(GL_BLEND);
        glDepthMask(GL_FALSE);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_CULL_FACE);
        glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
        glDisable(GL_STENCIL_TEST);
		glEnable(GL_SCISSOR_TEST); // box is set by ScissorStack init in GUI each frame
        glActiveTexture(GL_TEXTURE0);
    }

    void GLSetup::restore()
    {
        // Restoring
        glViewport(mViewport[0], mViewport[1], mViewport[2], mViewport[3]);
        setCapability(GL_BLEND, mBlend);
        glDepthMask(mDepthMask);
        glBlendFunc(mBlendSrc, mBlendDst);
        glUseProgram(mShaderProgram);
        glBindVertexArray(mVertexArrayObject);
        setCapability(GL_CULL_FACE, mCulling);
        glColorMask(mColorWritemask[0], mColorWritemask[1], mColorWritemask[2], mColorWritemask[3]);
        setCapability(GL_STENCIL_TEST, mStencilTest);
		setCapability(GL_SCISSOR_TEST, mScissorTest);
		glScissor(mScissorBox[0], mScissorBox[1], mScissorBox[2], mScissorBox[3]);
		glActiveTexture(mTextureSlot);
    }

    void GLSetup::setCapability(GLenum cap, GLboolean enable) const
    {
        if (enable)
        {
            glEnable(cap);
        }
        else
        {
            glDisable(cap);
        }
    }
}
