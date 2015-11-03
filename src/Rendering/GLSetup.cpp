//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "GLSetup.h"

namespace eyegui
{
	void GLSetup::init()
	{
		ogl_LoadFunctions();
	}

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
		glGetBooleanv(GL_TEXTURE_2D, &mTexture2D);
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &mActiveTexture);
		glGetBooleanv(GL_CULL_FACE, &mCulling);

		// Setting
		glViewport(viewportX, viewportY, viewportWidth, viewportHeight);
		glEnable(GL_BLEND);
		glDepthMask(GL_FALSE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glEnable(GL_TEXTURE_2D);
		glActiveTexture(GL_TEXTURE0);
		glEnable(GL_CULL_FACE);
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
		setCapability(GL_TEXTURE_2D, mTexture2D);
		glActiveTexture(mActiveTexture);
		setCapability(GL_CULL_FACE, mCulling);
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
