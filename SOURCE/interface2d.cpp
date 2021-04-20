#include "interface2d.h"

std::shared_ptr<nico::Texture> Boussole::image;

Boussole::Boussole(nico::Window* win, nico::Camera* cam) : Object2d(image.get())
{
	this->win = win;
	this->cam = cam;
}

void Boussole::draw(nico::Shader* shader)
{
	scale = glm::vec2((float)win->getHeight() / 30.0f);
	position = glm::vec2(win->getWidth() * .5f, win->getHeight() - scale.y);

	Object2d::draw(shader);
}

void Boussole::createTexture()
{
	image = std::make_shared<nico::Texture>(NICO_TEXTURES_PATH"halo.png");
}
