#include "Model.h"
#include "Object3d.h"

using namespace nico;

nico::Model::Model(const char* path)
{
	loadModel(path);
}

void nico::Model::draw(Shader* shader)
{
	for (size_t i = 0; i < meshes.size(); i++)
	{
		meshes[i].draw(shader);
	}
}

void nico::Model::loadModel(const char* path)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(path, 
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_PreTransformVertices | 
		aiProcess_CalcTangentSpace | 
		aiProcess_ForceGenNormals);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR with assimp in the nico::model class :\n" << importer.GetErrorString() << "\n";
		return;
	}
	directory = std::string(path).substr(0, std::string(path).find_last_of('/'));
	processNode(scene->mRootNode, scene);
}

void nico::Model::processNode(aiNode* node, const aiScene* scene)
{
	// process all the node's meshes (if any)
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}
	// then do the same for each of its children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		processNode(node->mChildren[i], scene);
	}
}

Mesh nico::Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		// process vertex positions, normals and texture coordinates

		vertex.positions = { mesh->mVertices[i].x,  mesh->mVertices[i].y , mesh->mVertices[i].z };
		vertex.normals = { mesh->mNormals[i].x,  mesh->mNormals[i].y , mesh->mNormals[i].z };

		if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
		{
			vertex.texturesUv = { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y };
		}
		else {
			vertex.texturesUv = { 0.0f, 0.0f };
		}

		vertex.tangents = { mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z };
		vertex.bitangents = { mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z };
		
		vertices.push_back(vertex);
	}
	// process indices
	//a face is indices to draw a single triangle in this case
	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];//goes trought all triangles indices
		/*if (face.mNumIndices < 3) {
			continue;
		}
		assert(face.mNumIndices == 3);*/

		for (unsigned int j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);//add all the face's indices in our vector of indices
		}
	}
	
	// process material
	if (mesh->mMaterialIndex >= 0)
	{
		textures = loadMaterialTextures(scene->mMaterials[mesh->mMaterialIndex]);
	}
	return nico::Mesh(vertices, indices, textures);
}

std::vector<Texture> nico::Model::loadMaterialTextures(aiMaterial* mat)
{
	std::vector<Texture> textures;
	//assimp aiTextureType textures loading
	for (uint32_t t = 1; t <= 17; t++)//there is 17 type of texture with assimp + 1 unknown type
	{
		if (mat->GetTextureCount(aiTextureType(t)) == 0) { continue; }
		aiString str;
		mat->GetTexture(aiTextureType(t), 0, &str);
		
		bool find = false;
		std::string texturePath(directory + "\\" + str.C_Str());

		for (size_t j = 0; j < texturesAlreadyLoaded.size(); j++)
		{
			if (texturePath.c_str() == texturesAlreadyLoaded[j].getPath()) {
				textures.push_back(texturesAlreadyLoaded[j]);
				break;
				find = true;
			}
		}
		if (!find) {
			textures.push_back(Texture(texturePath.c_str(), texParameters(), aiTextureType(t)));
		}
	}
	
	return textures;
}