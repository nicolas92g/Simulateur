#include "Line.h"

nico::Line::Line(glm::vec3 position_, glm::vec3 vector_, float size_) : VAO(){

	position = position_;
	vector = glm::normalize(vector_);
	size = size_;

	std::vector<float> vertices = {
		position.x, position.y, position.z,
		position.x + vector.x, position.y + vector.y, position.z + vector.z
	};

	VertexBuffer<float> VBO(&vertices, 3);
	VBO.addLayout(NICO_POSITIONS);
}

void nico::Line::draw(Shader* shader)
{
	VAO.bind();
	std::vector<float> vertices = {
		position.x, position.y, position.z,
		position.x + (vector.x * size), position.y + (vector.y * size), position.z + (vector.z * size)
	};

	VertexBuffer<float> VBO(&vertices, 3);
	VBO.addLayout(NICO_POSITIONS);

	shader->use();
	glDrawArrays(GL_LINES, 0, 2);
}

void nico::Line::setPosition(glm::vec3 position_)
{
	position = position_;
}

void nico::Line::setVector(glm::vec3 vector_)
{
	vector = glm::normalize(vector_);
}

void nico::Line::setSize(float size_)
{
	size = size_;
}
