#include "imageObject.h"
#include <nico/renderer.h>



nico::ImageMesh::ImageMesh(const char* filepath, uint32_t repeat, texParameters param) : VAO()
{
	repeat++;
	std::vector<float> vertex = {
		//positions			//normals			//texture
		 0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		 0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		 0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f
	};
	for (size_t i = 0; i < vertex.size(); i++) {
		if (((i + 1) % 8) == 7 or ((i + 1) % 8) == 0) {
			vertex[i] *= (float)repeat;
		}
	}
	
	nico::VertexBuffer<float> buffer(&vertex, 8);

	buffer.addLayout(NICO_POSITIONS);
	buffer.addLayout(NICO_NORMALS);
	buffer.addLayout(NICO_TEXTURESUV);
	
	image = new Texture(filepath, param);
}

nico::ImageMesh::ImageMesh(Texture* image, uint32_t repeat)
{
	repeat++;
	std::vector<float> vertex = {
		//positions			//normals			//texture
		 0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 1.0f,
		-0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		 0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		 0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0f, 0.0f,
		-0.5f, 0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 1.0f,
		-0.5f,-0.5f, 0.0f,	0.0f, 1.0f, 0.0f,	0.0f, 0.0f
	};
	for (size_t i = 0; i < vertex.size(); i++) {
		if (((i + 1) % 8) == 7 or ((i + 1) % 8) == 0) {
			vertex[i] *= (float)repeat;
		}
	}

	nico::VertexBuffer<float> buffer(&vertex, 8);

	buffer.addLayout(NICO_POSITIONS);
	buffer.addLayout(NICO_NORMALS);
	buffer.addLayout(NICO_TEXTURESUV);
	this->image = image;
}

nico::ImageMesh::~ImageMesh()
{
	delete image;
}

void nico::ImageMesh::draw(nico::Shader* shader)
{
	shader->use();
	VAO.bind();

	glActiveTexture(GL_TEXTURE0);
	image->bind();

	glActiveTexture(GL_TEXTURE1);
	shader->set("specular", 1);
	shader->set("normalMap", 1);
	glBindTexture(GL_TEXTURE_2D, 0);
	

	glDrawArrays(GL_TRIANGLES, 0, 6);
}