#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <nico/vertexBuffer.h>
#include <nico/Texture.h>
#include <nico/Shader.h>
#include <nico/Classes.h>
#include <nico/renderer.h>

namespace nico {
	/**
	 * @brief Mesh is some Vertices with location, indices, textures
	 */
	class Mesh : public DrawableObject
	{
	public:
		/**
		 * @brief create a mesh with all the data put in parameter
		 * \param verts
		 * \param inds
		 * \param texts
		 */
		Mesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds, const std::vector<Texture>& texts);
		/**
		 * @brief draw the mesh with the shader put in param
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief if true then all the meshes will be rendered as lines and not as triangles
		 */
		static bool renderLines;
	private:
		uint32_t numberOfIndices;
		std::vector<Texture> textures;

		uint32_t VAO;
		uint32_t VBO;
		uint32_t EBO;

		void setup(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& indices);
		void bindTextures(Shader* shader);
	};
}