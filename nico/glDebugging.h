#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>


#define glCheckError() nico::glCheckError_(__FILE__, __LINE__)
namespace nico {
	GLenum glCheckError_(const char* file, int line);
	void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id, GLenum severity,
		GLsizei length, const char* message, const void* userParam);
}
