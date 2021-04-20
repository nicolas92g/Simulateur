#include "interface2d.h"

std::shared_ptr<nico::Texture> Boussole::image;

Boussole::Boussole(nico::Window* win, nico::Camera* cam, nico::TextRenderer* text) : Object2d(image.get())
{
	this->win = win;
	this->cam = cam;
	this->text = text;

	mixedColor = glm::vec3(.0f);
	direction = glm::vec2(.5f);
}

void Boussole::draw(nico::Shader* shader)
{
	scale = glm::vec2((float)win->getHeight() * 0.01f);

	float offset = (glm::dot(cam->getLook(), glm::vec3(direction.y, 0, -direction.x))) * (win->getWidth() * .75f / cam->getFov());

	if (glm::length(cam->getLook() + glm::vec3(-direction.x, 0, -direction.y)) > 1)
		return;

	position = glm::vec2(win->getWidth() * .5f + offset, win->getHeight() - scale.y);

	if(name.size())
		text->print(name, position.x - text->calculateTextRenderingSize(name, 15.0f).x * .5f, position.y - 20.0f, 15.0f);

	Object2d::draw(shader);
}

void Boussole::setDirection(glm::vec3 dir)
{
	direction = glm::normalize(glm::vec2(dir.x, dir.z));
}

void Boussole::setName(const char* name)
{
	this->name = name;
}

const char* Boussole::getName() const
{
	return this->name.c_str();
}

const glm::vec2& Boussole::getDirection() const
{
	return direction;
}

void Boussole::createTexture()
{
	image = std::make_shared<nico::Texture>(NICO_TEXTURES_PATH"halo.png");
}
