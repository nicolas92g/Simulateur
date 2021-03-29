#pragma once
#include "Classes.h"
#include "Shader.h"
#include "Texture.h"
#include "Camera.h"
#include "vertexBuffer.h"

namespace nico{
	/**
	 * @class Billboard 
	 * @brief this Class allow to display some 2d textures in a 3d location 
	 * but in a way that the texture is always in front of the camera
	 */
	class Billboard
	{
	public:
		/**
		 * @brief create the billboard object
		 * \param texture2d
		 */
		Billboard(Texture* texture2d, Camera* cam);
		/**
		 * @brief set the array of billboards to draw
		 * \param vertices
		 */
		void setLocations(std::vector<float>* vertices);
		/**
		 * @brief draw all the billboards
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief set the texture to put on the billboard
		 * \param texture2d
		 */
		void assignTexture(Texture* texture2d);
	protected:
		Texture* image;
		Camera* cam;

		VertexArray VAO;
		VertexBuffer<float> VBO;
		std::vector<float> locs;
	};
}
