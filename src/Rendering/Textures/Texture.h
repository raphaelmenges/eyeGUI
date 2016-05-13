//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Abstract encapsulation of the OpenGL texture creation and usage.

#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "include/eyeGUI.h"

#include "Defines.h"
#include "externals/OpenGLLoader/gl_core_3_3.h"

#include <string>
#include <vector>

namespace eyegui
{
    class Texture
    {
    public:

        enum class Filtering
        {
            NEAREST, LINEAR
        };

        enum class Wrap
        {
            CLAMP, BORDER, MIRROR, REPEAT
        };

        // Constructor for pixel based image
        Texture();

        // Destructor
        virtual ~Texture() = 0;

        // Bind texture to slot for rendering
        void bind(uint slot = 0) const;

        // Getter for width and height
        uint getWidth() const;
        uint getHeight() const;

        // Getter for aspect ratio
        float getAspectRatio() const;

    protected:

        // Create OpenGL texture (something like initialization)
		void createOpenGLTexture(unsigned char const * pData, Filtering filtering, Wrap wrap, uint width, uint height, uint channelCount, GLenum format, GLenum internalFormat, bool flipY, std::string filepath);

		// Flip pixels vertically
		void flipPixelsY(unsigned char const * pData, unsigned char* pFlippedData, uint width, uint height, uint channelCount) const;

    private:

        // Members
        GLuint mTexture;
        uint mWidth;
        uint mHeight;
    };
}

#endif // TEXTURE_H_
