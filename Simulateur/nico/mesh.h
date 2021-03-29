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
		Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds, std::vector<Texture> texts);
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
		std::vector<Vertex> Vertices;
		std::vector<uint32_t> indices;
		std::vector<Texture> textures;

		VertexArray* VAO;
		VertexBuffer<Vertex> *VBO;
		indicesBuffer<unsigned int>* EBO;

		void setup();
	};
}