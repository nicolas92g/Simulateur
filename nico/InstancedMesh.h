#pragma once
#include "mesh.h"

namespace nico{
	/**
	 * @brief utilisée pour afficher des modeles 3D par instanciation mais ca marche pas encore
	 */
	class InstancedMesh : public DrawableObject{
	public:
		InstancedMesh(
			const std::vector<Vertex>& verts, 
			const std::vector<uint32_t>& inds,
			const std::vector<Texture>& texts, 
			const std::vector<glm::mat4>& models = {});
		void draw(Shader* shader);
		void setModels(const std::vector<glm::mat4>& models);

	protected:
		uint32_t numberOfIndices;
		uint32_t numberOfInstances;
		std::vector<Texture> textures;

		uint32_t VAO;//Vertex Array Object
		uint32_t VBO;//Vertex Buffer Object
		uint32_t EBO;//Index Buffer Object
		uint32_t IBO;//Instanced Buffer Object

		void setup(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& indices);
		void bindTextures(Shader* shader);
	};


}
