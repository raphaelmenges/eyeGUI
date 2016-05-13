//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Texture.h"

#include "src/Utilities/OperationNotifier.h"

#include <fstream>

namespace eyegui
{
    Texture::Texture()
    {
        // Initialize members
        mTexture = 0;
        mWidth = 0;
        mHeight = 0;
    }

    Texture::~Texture()
    {
        // Delete texture
        glDeleteTextures(1, &mTexture);
    }

    void Texture::bind(uint slot) const
    {
        // Choose slot
        glActiveTexture(GL_TEXTURE0 + slot);

        // Bind texture
        glBindTexture(GL_TEXTURE_2D, mTexture);
    }

    uint Texture::getWidth() const
    {
        return mWidth;
    }

    uint Texture::getHeight() const
    {
        return mHeight;
    }

    float Texture::getAspectRatio() const
    {
        return ((float)mWidth) / ((float)mHeight);
    }

    void Texture::createOpenGLTexture(unsigned char const * pData, Filtering filtering, Wrap wrap, uint width, uint height, uint channelCount, GLenum format, GLenum internalFormat, bool flipY, std::string filepath)
    {
        // Save members
        mWidth = width;
        mHeight = height;

		// Flip if necessary
		std::vector<uchar> flippedData;
		if (flipY)
		{
			flippedData.resize(width * height * channelCount);
			flipPixelsY(pData, flippedData.data(), width, height, channelCount);
		}

        // Create OpenGL texture
        glActiveTexture(GL_TEXTURE0);
        glGenTextures(1, &mTexture);
        glBindTexture(GL_TEXTURE_2D, mTexture);

        // Wrapping
        switch (wrap)
        {
        case Wrap::CLAMP:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            break;
        case Wrap::BORDER:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
            break;
        case Wrap::MIRROR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
            break;
        case Wrap::REPEAT:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            break;
        }

        // Load it to GPU
		if (flipY)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat , mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, flippedData.data());
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, mWidth, mHeight, 0, format, GL_UNSIGNED_BYTE, pData);
		}

        // Filtering
        switch (filtering)
        {
        case Filtering::LINEAR:
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            break;
        case Filtering::NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            break;
        }

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

	void Texture::flipPixelsY(unsigned char const * pData, unsigned char* pFlippedData, uint width, uint height, uint channelCount) const
	{
		// Go over lines
		for (uint i = 0; i < height; i++)
		{
			// Go over columns
			for (uint j = 0; j < width; j++)
			{
				// Go over channels
				for (uint k = 0; k < channelCount; k++)
				{
					pFlippedData[i * width * channelCount + j * channelCount + k] = pData[(height - 1 - i) * width * channelCount + j * channelCount + k];
				}
			}
		}
	}	
}
