//============================================================================
// Distributed under the MIT License. (See accompanying file LICENSE
// or copy at https://github.com/raphaelmenges/eyeGUI/blob/master/src/LICENSE)
//============================================================================

// Author: Raphael Menges (https://github.com/raphaelmenges)
// Encapsulation of the OpenGL shader creation and usage.

#ifndef SHADER_H_
#define SHADER_H_

#include "externals/OpenGLLoader/gl_core_3_3.h"
#include "externals/GLM/glm/glm.hpp"

#include <string>
#include <map>

namespace eyegui
{
	class Shader
	{
	public:

		// Constructor
		Shader(char const * const pVertexShader, char const * const pFragmentShader);

		// Destructor
		virtual ~Shader();

		// Bind shader for filling uniforms and rendering
		void bind() const;

		// Fill value for rendering
		void fillValue(std::string location, const float rValue) const;
		void fillValue(std::string location, const glm::vec2& rValue) const;
		void fillValue(std::string location, const glm::vec3& rValue) const;
		void fillValue(std::string location, const glm::vec4& rValue) const;
		void fillValue(std::string location, const glm::mat4x4& rValue) const;

		// Getter for handle
		GLuint getShaderProgram() const;

	private:

		// Evaluate shader compiling log
		void evaluateShaderLog(GLuint handle) const;

		// Member
		GLuint mShaderProgram;
		std::map<std::string, int> mUniformLocationCache;
	};
}

#endif // SHADER_H_
