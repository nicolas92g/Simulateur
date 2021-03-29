#include "Vertex.h"

nico::Vertex::Vertex()
{
	positions =		glm::vec3(0.0f, 0.0f, 0.0f);
	normals =		glm::vec3(0.0f, 0.0f, 0.0f);
	texturesUv =	glm::vec2(0.0f, 0.0f);
	tangents =		glm::vec3(0.0f, 0.0f, 0.0f);
	bitangents =	glm::vec3(0.0f, 0.0f, 0.0f);
}

nico::Vertex::Vertex(glm::vec3 pos)
{
	positions = pos;
	normals = glm::vec3(0);
	texturesUv = glm::vec3(0);
	tangents = glm::vec3(0);
	bitangents = glm::vec3(0);
}

nico::Vertex::Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv)
{
	positions = pos;
	normals = norm;
	texturesUv = uv;
	tangents = glm::vec3(0);
	bitangents = glm::vec3(0);
}

nico::Vertex::Vertex(glm::vec3 pos, glm::vec3 norm, glm::vec2 uv, glm::vec3 tang, glm::vec3 bi)
{
	positions = pos;
	normals = norm;
	texturesUv = uv;
	tangents = tang;
	bitangents = bi;
}
