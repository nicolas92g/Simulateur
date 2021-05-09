#include "InstancedMesh.h" 

nico::InstancedMesh::InstancedMesh(const std::vector<Vertex>& verts, const std::vector<uint32_t>& inds,
	const std::vector<Texture>& texts, const std::vector<glm::mat4>& models) : EBO(0), VBO(0), VAO(0), IBO(0)
{
	textures = texts;
	setup(verts, inds);
	numberOfIndices = inds.size();
	if (!models.empty())
		setModels(models);

	glGenBuffers(1, &IBO);
	numberOfInstances = 0;
}

void nico::InstancedMesh::draw(Shader* shader)
{
	if (numberOfInstances < 1) return;
	shader->use();
	shader->set("instanced", true);
	bindTextures(shader);
	glBindVertexArray(VAO);
	glDrawElementsInstanced(GL_TRIANGLES, numberOfIndices, GL_UNSIGNED_INT, (void*)0, numberOfInstances);
	shader->set("instanced", false);
}

void nico::InstancedMesh::setModels(const std::vector<glm::mat4>& models)
{
	glBindVertexArray(VAO);

	//glDestroyBuffer(1, &IBO);
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ARRAY_BUFFER, IBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * models.size(), models.data(), GL_STATIC_DRAW);

	 size_t vec4Size = sizeof(glm::vec4);

	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)0);
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(1 * vec4Size));
	glEnableVertexAttribArray(7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(2 * vec4Size));
	glEnableVertexAttribArray(8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, 4 * vec4Size, (void*)(3 * vec4Size));

	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
	glVertexAttribDivisor(8, 1);

	glBindVertexArray(0);
}


void nico::InstancedMesh::bindTextures(Shader* shader)
{
	bool normalMap(false);
	bool specularMap(false);
	bool metalMap(false);

	for (size_t i = 0; (i < textures.size()); i++)
	{
		textures[i].useTexture((int)i);

		std::string samplerName;
		switch (textures[i].type) {
		case aiTextureType::aiTextureType_DIFFUSE:
			samplerName = "diffuse";
			break;
		case aiTextureType::aiTextureType_SPECULAR:
			samplerName = "specular";
			specularMap = true;
			break;
		case aiTextureType::aiTextureType_SHININESS:
			samplerName = "roughness";
			metalMap = true;
			break;
		case aiTextureType::aiTextureType_HEIGHT:
			samplerName = "normalMap";
			normalMap = true;
			break;

		case aiTextureType::aiTextureType_BASE_COLOR:
			samplerName = "diffuse";
			break;
		case aiTextureType::aiTextureType_AMBIENT_OCCLUSION:
			samplerName = "ao";
			break;
		default:
#				ifndef NDEBUG
			std::cout << "texture type : " << textures[i].type << " is not treated\n\r";
#				endif // DEBUG
			break;
		}
		shader->set(samplerName.c_str(), (int)i);
		textures[i].bind();
	}

	if (!normalMap)
		shader->set<int>("normalMap", 0);
	if (!specularMap)
		shader->set<int>("specular", 0);
	if (!metalMap)
		shader->set<int>("metallic", 0);


	glActiveTexture(GL_TEXTURE0);
}


void nico::InstancedMesh::setup(const std::vector<Vertex>& Vertices, const std::vector<uint32_t>& indices)
{
	//create vertex array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//create vertex buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), Vertices.data(), GL_STATIC_DRAW);

	//add layouts
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(0 * sizeof(float)));

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(Vertex), (void*)(6 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(8 * sizeof(float)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(Vertex), (void*)(11 * sizeof(float)));

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}