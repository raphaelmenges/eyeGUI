//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)

#include "Shader.h"

#include "externals/GLM/glm/gtc/type_ptr.hpp"
#include "OperationNotifier.h"

#include <fstream>

namespace eyegui
{
    Shader::Shader(char const * const pVertexShaderSource, char const * const pFragmentShaderSource)
    {
        // Vertex shader
        GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &pVertexShaderSource, NULL);
        glCompileShader(vertexShader);
        evaluateShaderLog(vertexShader);

        // Fragment shader
        GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &pFragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        evaluateShaderLog(fragmentShader);

        // Create program
        mShaderProgram = glCreateProgram();
        glAttachShader(mShaderProgram, vertexShader);
        glAttachShader(mShaderProgram, fragmentShader);
        glLinkProgram(mShaderProgram);

        // Delete shader
        glDetachShader(mShaderProgram, fragmentShader);
        glDetachShader(mShaderProgram, vertexShader);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        // Extract all uniforms of shader
        int total = -1;
        glGetProgramiv(mShaderProgram, GL_ACTIVE_UNIFORMS, &total);
        for (int i = 0; i < total; i++)
        {
            // Prepare some values
            int nameLength = -1;
            int number = -1;
            GLenum type = GL_ZERO;
            char name[100];

            // Get information about uniform
            glGetActiveUniform(mShaderProgram, GLuint(i), sizeof(name) - 1, &nameLength, &number, &type, name);

            // Add end of string
            name[nameLength] = 0;

            // Save mapping from name to location in map
            mUniformLocationCache[name] = glGetUniformLocation(mShaderProgram, name);
        }
    }

    Shader::~Shader()
    {
        glDeleteProgram(mShaderProgram);
    }

    void Shader::bind() const
    {
        glUseProgram(mShaderProgram);
    }

    void Shader::fillValue(std::string location, const float rValue) const
    {
        glUniform1fv(mUniformLocationCache.at(location), 1, &rValue);
    }

    void Shader::fillValue(std::string location, const glm::vec2& rValue) const
    {
        glUniform2fv(mUniformLocationCache.at(location), 1, glm::value_ptr(rValue));
    }

    void Shader::fillValue(std::string location, const glm::vec3& rValue) const
    {
        glUniform3fv(mUniformLocationCache.at(location), 1, glm::value_ptr(rValue));
    }

    void Shader::fillValue(std::string location, const glm::vec4& rValue) const
    {
        glUniform4fv(mUniformLocationCache.at(location), 1, glm::value_ptr(rValue));
    }

    void Shader::fillValue(std::string location, const glm::mat4x4& rValue) const
    {
        glUniformMatrix4fv(mUniformLocationCache.at(location), 1, GL_FALSE, glm::value_ptr(rValue));
    }

    GLuint Shader::getShaderProgram() const
    {
        return mShaderProgram;
    }

    void Shader::evaluateShaderLog(GLuint handle) const
    {
        // Get length of compiling log
        GLint log_length = 0;
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);

        if (log_length > 1)
        {
            // Copy log to chars
            GLchar *log = new GLchar[log_length];
            glGetShaderInfoLog(handle, log_length, NULL, log);

            // Print it
            throwError(
                OperationNotifier::Operation::BUG,
               "A shader throws errors at compilation:\n" + std::string(log));

            // Delete chars
            delete[] log;
        }
    }
}
