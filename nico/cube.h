#pragma once
#include <glm/glm.hpp>
#include <nico/Classes.h>
#include <nico/vertexBuffer.h>
#include <nico/Shader.h>

namespace nico {
	/**
	 * @brief this describe a cube mesh without textures
	 */
	class CubeMesh : public DrawableObject
	{
	public:
		/**
		 * @brief create the mesh
		 */
		CubeMesh();
		/**
		 * @brief draw the cube 
		 * you have to bind a texture before
		 * \param shader_
		 */
		void draw(nico::Shader* shader_);

	private:
		uint32_t VAO;
	};
};