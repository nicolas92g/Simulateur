#include "Object3d.h"

using namespace glm;

nico::Object3d::Object3d(DrawableObject* objectTodraw, vec3 position, vec3 scale, vec3 rotationAxe, float angle)
{
	this->position = position;

	//check if scale is not (0,0,0)
	if (scale != vec3(0.0f))
		this->scale = scale;
	else
		this->scale = vec3(1.0f);

	//check if axe is not (0,0,0)
	if (rotationAxe != vec3(0.0f))
		this->rotationAxe = rotationAxe;
	else
		this->rotationAxe = vec3(0, 1, 0);

	this->rotationAngle = angle;

	this->update();
	this->object = objectTodraw;
}

void nico::Object3d::update()
{
	model = translate(position) * rotate(rotationAngle, rotationAxe) * glm::scale(scale);
	if (object != nullptr)
		object->update();
}

void nico::Object3d::draw(Shader* shader)
{
	shader->use();
	shader->set("model", model);
	object->draw(shader);
}

void nico::Object3d::assignDrawableObject(DrawableObject* obj)
{
	object = obj;
}

void nico::Object3d::modifyValues(vec3 position, vec3 scale, vec3 rotationAxe, float angle)
{
	setPos(position);
	setScale(scale);
	setRotation(rotationAxe, angle);
	this->update();
}

void nico::Object3d::setPos(vec3 pos)
{
	position = pos;
}

void nico::Object3d::setScale(vec3 scale)
{
	this->scale = scale;
}

void nico::Object3d::setRotation(vec3 rotAxe, float angleInRadians)
{
	setRotationAxe(rotAxe);
	setRotationAngle(angleInRadians);
}

void nico::Object3d::setRotationAxe(vec3 rotAxe)
{
	if (rotAxe != vec3(0.0f))
		rotationAxe = rotAxe;
}

void nico::Object3d::setRotationAngle(float angle)
{
	rotationAngle = angle;
}

vec3 nico::Object3d::getPos() const
{
	return position;
}

vec3 nico::Object3d::getScale() const
{
	return scale;
}

vec3 nico::Object3d::getRotationAxe() const
{
	return rotationAxe;
}

glm::mat4 nico::Object3d::getWorldTransformation() const
{
	return model;
}

float nico::Object3d::getRotationAngle() const
{
	return rotationAngle;
}
