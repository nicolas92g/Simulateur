#pragma once
#include <nico/Shader.h>
#include <nico/mesh.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <nico/window.h>
#include <nico/Classes.h>
#include "Object3d.h"
#include <thread>
#include <nico/mesh.h>

namespace nico {
	/**
	 * @brief a model is an array of meshes
	 * that is loaded with assimp lib
	 * this class is only compatible with .obj files + .mtl
	 */
	class Model : public DrawableObject
	{
	public:
		/**
		 * @brief create a Model loading a .obj file
		 * \param path
		 */
		Model(const char* path);
		/**
		 * @brief draw all the meshes using shader
		 * \param shader
		 */
		void draw(Shader* shader);

	private:
		std::vector<Mesh> meshes;
		std::string directory;
		
		//this is to avoid loading many times the same texture file
		std::vector<Texture> texturesAlreadyLoaded;

		void loadModel(const char* path);
		void processNode(aiNode* node, const aiScene* scene);
		Mesh processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat);
	};
};