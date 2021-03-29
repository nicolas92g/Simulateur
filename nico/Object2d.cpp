#include "Object2d.h"

nico::Object2d::Object2d(Texture* image, glm::vec2 position, glm::vec2 scale, float rotAngle, glm::vec3 axe)
{
	this->texture = image;
	this->position = position;
	this->scale = scale;
	this->layer = 0;
	rotationAngle = rotAngle;
	rotationAxe = axe;

	mixedColor = glm::vec3(1);
	transparency = 1.0f;
}

void nico::Object2d::assignTexture(Texture* image)
{
	if (image != nullptr)
		texture = image;
}

void nico::Object2d::draw(Shader* shader)
{
	glActiveTexture(GL_TEXTURE0);
	texture->bind();
	preDrawInstructions(shader);
	
	object->draw(shader);
}

bool nico::Object2d::operator<(Object2d other)
{
	return this->layer < other.getLayer();
}

void nico::Object2d::setPosition(glm::vec2 pos)
{
	position = pos;
}

void nico::Object2d::setScale(glm::vec2 scale)
{
	this->scale = scale;
}

void nico::Object2d::setRelativeScale(glm::vec2 relativeScale)
{
	scale.x = texture->getWidth() * relativeScale.x * .5;
	scale.y = texture->getHeight() * relativeScale.y * .5;
}

void nico::Object2d::setRotation(float angle, glm::vec3 axe)
{
	rotationAngle = angle;
	rotationAxe = axe;
}

void nico::Object2d::setTransparency(float transparency)
{
	this->transparency = transparency;
}

void nico::Object2d::setMultiplyColor(glm::vec3 color)
{
	this->mixedColor = color;
}

void nico::Object2d::setLayer(unsigned int layer)
{
	this->layer = layer;
}

glm::vec2 nico::Object2d::getPosition() const
{
	return position;
}

glm::vec2 nico::Object2d::getScale() const
{
	return scale;
}

float nico::Object2d::getRotationAngle() const
{
	return rotationAngle;
}

glm::vec3 nico::Object2d::getRotationAxe() const
{
	return rotationAxe;
}

float nico::Object2d::getTransparency() const
{
	return transparency;
}

glm::vec3 nico::Object2d::getMixedColor() const
{
	return mixedColor;
}

unsigned int nico::Object2d::getLayer() const
{
	return this->layer;
}

void nico::Object2d::CreatePlane()
{
	object = new Plane();
}

void nico::Object2d::DestroyPlane()
{
	delete object;
}

void nico::Object2d::preDrawInstructions(Shader* shader)
{
	glDepthFunc(GL_ALWAYS);
	shader->set("model", glm::translate(glm::vec3(position, 0)) * glm::rotate(glm::radians(rotationAngle), rotationAxe) * glm::scale(glm::vec3(scale, 1)));
	shader->set("mixedColor", mixedColor);
	shader->set("transparency", transparency);
}

nico::Plane* nico::Object2d::object;