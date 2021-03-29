#pragma once
#include <glm/glm.hpp>

namespace nico{
	/**
	 * @brief represent a 3D vertex and all it's data
	 */
	struct Vertex {
		glm::vec3 positions;
		glm::vec3 normals;
		glm::vec2 texturesUv;
		glm::vec3 tangents;
		glm::vec3 bitangents;

		Vertex();
		Vertex(glm::vec3 pos);
		Vertex(glm::vec3 pos, glm::vec3 normals, glm::vec2 uv);
		Vertex(glm::vec3 pos, glm::vec3 normals, glm::vec2 uv, glm::vec3 tangents, glm::vec3 bitangents);
	};
}
