#pragma once
#include <glm/gtx/transform.hpp>
#include <nico/Shader.h>
#include <nico/Texture.h>
#include <nico/vertexBuffer.h>
#include <nico/Classes.h>

namespace nico {
	/**
	 * @brief allow to render a texture in 3D as a plane
	 */
	class ImageMesh : public DrawableObject
	{
	public:
		/**
		 * @brief create a Texture and create a mesh
		 * repeat is how many times the texture is repeated one the plane
		 * \param filepath
		 * \param repeat
		 * \param param
		 */
		ImageMesh(const char* filepath, uint32_t repeat = 0, texParameters param = texParameters());
		/**
		 * @brief bind a texture and create a plane
		 * repeat is how many times the texture is repeated one the plane
		 * \param image
		 * \param repeat
		 */
		ImageMesh(Texture* image, uint32_t repeat);
		/**
		 * @brief destroy the texture
		 */
		~ImageMesh();
		/**
		 * @brief draw the plane and bind the texture
		 * \param shader
		 */
		void draw(nico::Shader* shader);
	
	private:
		nico::VertexArray VAO;
		nico::Texture* image;
	};
}

