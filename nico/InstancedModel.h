#pragma once
#include "InstancedMesh.h"

namespace nico{
	class InstancedModel{
	public:
		/**
		 * @brief create a Model loading a .obj file
		 * \param path
		 */
		InstancedModel(const char* path);
		/**
		 * @brief set the models matrices to use for each instance
		 */
		void setModels(std::vector<glm::mat4>& models);
		/**
		 * @brief draw all the meshes using shader
		 * \param shader
		 */
		void draw(Shader* shader);
	private:
		std::vector<nico::InstancedMesh> meshes;
		std::string directory;

		//this is to avoid loading many times the same texture file
		std::vector<Texture> texturesAlreadyLoaded;

		void loadModel(const char* path);
		void processNode(aiNode* node, const aiScene* scene);
		nico::InstancedMesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat);
	};
}
