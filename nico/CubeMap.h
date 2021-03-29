#pragma once
#include <nico/openGlObject.h>
#include <GLAD/glad.h>
//#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>
#include "Classes.h"
#include "vertexBuffer.h"
#include "Camera.h"
#include "cube.h"
#include <config.hpp>

namespace nico{
	/**
	 * @brief this allow to create a cubeMap texture easily and draw it as a skyMap
	 */
	class CubeMap : public GlObject, public DrawableObject
	{
	public:
		/**
		 * this load a cubemap with 6 images
		 * this need their full path
		 * \param filesPath
		 */
		CubeMap(std::vector<std::string> filesPath);
		/**
		 * this load a cubemap with 6 images
		 * need a directory full path and the textures path relatively to this directory
		 * \param directory
		 * \param relativeFilesPath
		 */
		CubeMap(std::string directory, std::vector<std::string> relativeFilesPath);
		/**
		 * this load a hdr texture, need it full path
		 * @param path
		 * @param resolution of the generated cubemap (this is limited by the hdr map so don't put 100000 if you have a 2x2 texture)
		 */
		CubeMap(std::string hdrTexturePath, uint32_t resolution = 512);

		void loadFiles(std::string directory, std::vector<std::string> relativeFilesPath);
		void loadFiles(std::vector<std::string> filesPath);
		void loadHdrFile(std::string hdrTexturePath, uint32_t resolution = 512);
		/**
		 * @brief the opengl texture TEXTURE_CUBEMAP
		 */
		void bind();
		/**
		 * @brief unbind a cubemap texture
		 */
		void unbind();
		/**
		 * @brief draw the cubemap as a skyMap on a cube
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief to which camera the cubemap have to be drawed
		 * \param cam
		 */
		void useCamera(Camera* cam);

	protected:
		VertexArray VAO;
		CubeMesh* cube;
		Camera* cam;

		bool HDR;
		bool renderable;

	private:
		void fillVAO();
	};
}
