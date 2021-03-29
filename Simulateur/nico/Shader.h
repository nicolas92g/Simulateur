#pragma once
#include <glad/glad.h>
#include <KHR/khrplatform.h>
#include <glm/glm.hpp>
#include <string>
#include <fstream>

#include <nico/nicoFunctions.h>
#include <fstream>
#include <nico/openGlObject.h>
#include <map>


namespace nico {
	/**
	 * @brief compile and use a shader program in glsl
	 */
	class Shader : public GlObject
	{
	public:
		/**
		 * @brief read all the files in inputs and compile them as glsl shaders
		 * errors of compiling are logs in the shell
		 * \param vertexPath
		 * \param fragmentPath
		 * \param geometryPath
		 */
		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		/**
		 * @brief compile strings as glsl shaders
		 * errors of compiling are logs in the shell
		 * \param vertexText
		 * \param fragmentText
		 * \param geometryText
		 */
		Shader(std::string vertexText, std::string fragmentText, std::string geometryText = "");
		/**
		 * @brief read all the files in inputs and compile them as glsl shaders
		 * errors of compiling are logs in the shell
		 * \param vertexPath
		 * \param fragmentPath
		 * \param geometryPath
		 */
		void loadShaderFrom(const char* vertexPath, const char* fragmentPath, const char* geometryPath = nullptr);
		/**
		 * @brief compile strings(in format const char*) as glsl shaders
		 * errors of compiling are logs in the shell
		 * \param vertexText
		 * \param fragmentText
		 * \param geometryText
		 */
		void compileShader(const char* vertexText, const char* fragmentText, const char* geometryText = nullptr);
		/**
		 * bind this shader
		 * need this line before a draw call to render with this shader
		 */
		void use() const;

		template<typename T>
		/**
		 * change the value of a uniform var in the shader
		 * types supported are : int, unsigned int, bool, float, glm::vec2, glm::vec3, glm::vec4, glm::mat4 
		 * \param name
		 * \param value
		 */
		void set(const char* name, T value);

	protected:
		void bind() {}
	};

	/*template<typename T>
	class Uniform {
		T value;
		Shader* shader;
		const char* name;
	public:
		Uniform(Shader *shader_, const char* name_) {
			shader = shader_;
			name = name_;
		}
		void operator=(T value_) {
			value = value_;
			shader->use();
			shader->set<T>(name,value_);
		}
		operator T() const { return value; }
	};*/
	
}
template<typename T>
inline void nico::Shader::set(const char* name, T value)
{
	static_assert(false, "type is not supported by nico::Shader class");
}
template <>
inline void nico::Shader::set(const char* name, int value)
{
	this->use();
	glUniform1i(glGetUniformLocation(id, name), value);
}
template <>
inline void nico::Shader::set(const char* name, unsigned int value)
{
	this->use();
	glUniform1ui(glGetUniformLocation(id, name), value);
}
template <>
inline void nico::Shader::set(const char* name, bool value)
{
	this->use();
	glUniform1i(glGetUniformLocation(id, name), value);
}
template <>
inline void nico::Shader::set(const char* name, float value)
{
	this->use();
	glUniform1f(glGetUniformLocation(id, name), value);
}
template <>
inline void nico::Shader::set(const char* name, glm::vec2 value)
{
	this->use();
	glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]);
}
template <>
inline void nico::Shader::set(const char* name, glm::vec3 value)
{
	this->use();
	glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]);
}
template <>
inline void nico::Shader::set(const char* name, glm::vec4 value)
{
	this->use();
	glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]);
}
template <>
inline void nico::Shader::set(const char* name, glm::mat4 value)
{
	this->use();
	glUniformMatrix4fv(glGetUniformLocation(id, name), 1, false, &value[0][0]);
}