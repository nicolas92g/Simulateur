#include "Shader.h"

//this constructor require filepath of shaders
nico::Shader::Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	loadShaderFrom(vertexPath, fragmentPath, geometryPath);
}

nico::Shader::Shader(std::string vertexText, std::string fragmentText, std::string geometryText)
{
	if(geometryText.size() < 3)
		compileShader(vertexText.c_str(), fragmentText.c_str(), geometryText.c_str());
	else
		compileShader(vertexText.c_str(), fragmentText.c_str(), nullptr);
}

//this function require filepath of shaders
void nico::Shader::loadShaderFrom(const char* vertexPath, const char* fragmentPath, const char* geometryPath)
{
	std::string vertex;
	if (!strings::filepathToString(vertex, vertexPath)) {
		std::cout << "error : can't find vertex shader's file at path :\n" << vertexPath << "\n";
		//assert(false);
	}
	std::string fragment;
	if (!strings::filepathToString(fragment, fragmentPath)) {
		std::cout << "error : can't find fragment shader's file at path :\n" << fragmentPath << "\n";
		//assert(false);
	}
	std::string geometry;
	const char* geo = nullptr;
	if (geometryPath != nullptr) {
		if (!strings::filepathToString(geometry, geometryPath)) {
			std::cout << "error : can't find geometry shader's file at path :\n" << geometryPath << "\n";
			//assert(false);
		}
		geo = geometry.c_str();
	}

	compileShader(vertex.c_str(), fragment.c_str(), geo);
}

void nico::Shader::use() const
{
	glUseProgram(id);
}

void nico::Shader::compileShader(const char* vertex, const char* fragment, const char* geometry)
{
	//vertex Shader
	uint32_t vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertex, NULL);
	glCompileShader(vertexShader);

	//check if vertex compilation success
	int* vertexCompilationSuccess = new int();
	char errorLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, vertexCompilationSuccess);
	if (!*vertexCompilationSuccess) {
		glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
		std::cout << "ERROR in the vertex shader \n" << errorLog << '\n';
		//assert(false);
	}

	uint32_t geometryShader;
	if (geometry != nullptr) {
		//geometry shader 
		geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(geometryShader, 1, &geometry, NULL);
		glCompileShader(geometryShader);

		//check if geometry compilation success
		int* geometryCompilationSuccess = new int();
		char errorLog[512];
		glGetShaderiv(geometryShader, GL_COMPILE_STATUS, geometryCompilationSuccess);
		if (!*geometryCompilationSuccess) {
			glGetShaderInfoLog(geometryShader, 512, NULL, errorLog);
			std::cout << "ERROR in the fragment shader \n" << errorLog << '\n';
			//assert(false);
		}
	}

	//fragment shader
	uint32_t fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragment, NULL);
	glCompileShader(fragmentShader);

	int* fragmentCompilationSuccess = new int();
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, fragmentCompilationSuccess);
	if (!*fragmentCompilationSuccess) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
		std::cout << "ERROR in the fragment shader \n" << errorLog << '\n';
		//assert(false);
	}
	//create a programm object
	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragmentShader);
	if(geometry != nullptr)
		glAttachShader(id, geometryShader);
	glLinkProgram(id);

	//verify if link was a success
	int linkSuccess;
	glGetProgramiv(id, GL_LINK_STATUS, &linkSuccess);
	if (!linkSuccess) {
		glGetProgramInfoLog(id, 512, NULL, errorLog);
		std::cout << "ERROR while linking shaders \n" << errorLog << '\n';
		//assert(false);
	}
	use();
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

