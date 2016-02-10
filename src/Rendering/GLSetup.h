//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Stores the current state of used OpenGL functions, sets the necesarry ones
// for rendering and restores to previous state after rendering. Texture slots
// are NOT restored because this would be expensive and probably meaningless
// when the user of this library is sane. Framebuffers are also not touched.

#ifndef GL_SETUP_H_
#define GL_SETUP_H_

#include "externals/OpenGLLoader/gl_core_3_3.h"

namespace eyegui
{
    class GLSetup
    {
    public:

        // Store current state and set own
        void setup(int viewportX, int viewportY, int viewportWidth, int viewportHeight);

        // Reset stored state
        void restore();

    private:

        // Set boolean capability
        void setCapability(GLenum cap, GLboolean enable) const;

        // Member
        GLint mViewport[4];
        GLboolean mBlend;
        GLboolean mDepthMask;
        GLint mBlendSrc;
        GLint mBlendDst;
        GLint mShaderProgram;
        GLint mVertexArrayObject;
        GLboolean mCulling;
        GLboolean mColorWritemask[4];
        GLboolean mStencilTest;
    };
}

#endif // GL_SETUP_H_
