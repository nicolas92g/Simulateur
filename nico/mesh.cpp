#include "mesh.h"

using namespace nico;
using namespace glm;

nico::Mesh::Mesh(std::vector<Vertex> verts, std::vector<uint32_t> inds, std::vector<Texture> texts) :
	Vertices(1)
{
	Vertices = verts;
	indices = inds;
	textures = texts;
	setup();
}

void nico::Mesh::draw(Shader* shader)
{
	shader->use();

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
			case aiTextureType::aiTextureType_SHININESS:
				samplerName = "roughness";
				metalMap = true;
				break;
			default:
				std::cout << "type : " << textures[i].type << "is not treated\n\r";
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

	//draw call :
	
	VAO->bind();
	if(renderLines)
		glDrawElements(GL_LINES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	else
		glDrawElements(GL_TRIANGLES, (GLsizei)indices.size(), GL_UNSIGNED_INT, 0);
	VAO->unbind();
}


void nico::Mesh::setup()
{
	VAO = new VertexArray();

	VBO = new VertexBuffer<Vertex>(&Vertices);

	VBO->addLayout({ 0,3 });
	VBO->addLayout({ 1,3 });
	VBO->addLayout({ 2,2 });
	VBO->addLayout({ 3,3 });
	VBO->addLayout({ 4,3 });

	EBO = new indicesBuffer<unsigned int>(indices);
	
	VAO->unbind();
}

bool nico::Mesh::renderLines = false;