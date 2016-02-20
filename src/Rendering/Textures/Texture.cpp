//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Texture.h"

#include "OperationNotifier.h"

#include <fstream>

namespace eyegui
{
    Texture::Texture()
    {
        // Initialize members
        mTexture = 0;
        mWidth = 0;
        mHeight = 0;
        mChannelCount = 0;
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

    uint Texture::getChannelCount() const
    {
        return mChannelCount;
    }

    void Texture::createOpenGLTexture(const std::vector<uchar>& rData, Filtering filtering, Wrap wrap, uint width, uint height, uint channelCount, std::string filepath)
    {
        // Save members
        mWidth = width;
        mHeight = height;
        mChannelCount = channelCount;

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
        switch (mChannelCount)
        {
        case 1:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mWidth, mHeight, 0, GL_RED, GL_UNSIGNED_BYTE, &rData[0]);
            break;
        case 3:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, &rData[0]);
            break;
        case 4:
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &rData[0]);
            break;
        default:
            throwWarning(OperationNotifier::Operation::IMAGE_LOADING, "Unknown number of color channels", filepath);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, mWidth, mHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, &rData[0]);
            break;
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
}
