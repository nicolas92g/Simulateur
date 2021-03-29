#include "Button.h"

nico::Button::Button(Window* win, Texture* buttonTexture, glm::vec2 position, glm::vec2 scale) : Object2d(buttonTexture, position, scale)
{
	this->win = win;
	Clickedtexture = buttonTexture;
	HoverTexture = buttonTexture;
	texture = buttonTexture;
}

nico::Button::Button(Window* win, glm::vec2 position, glm::vec2 scale) : Object2d(nullptr, position, scale)
{
	if (defaultTextures[0] == nullptr) {
		defaultTextures[0] = new Texture(NICO_TEXTURES_PATH"button.png");
		defaultTextures[1] = new Texture(NICO_TEXTURES_PATH"hover.png");
		defaultTextures[2] = new Texture(NICO_TEXTURES_PATH"clicked.png");
	}
	//texture = defaultTextures[0];
	//HoverTexture = defaultTextures[1];
	//Clickedtexture = defaultTextures[2];

	texture = new Texture(NICO_TEXTURES_PATH"button.png");
	HoverTexture = new Texture(NICO_TEXTURES_PATH"hover.png");
	Clickedtexture = new Texture(NICO_TEXTURES_PATH"clicked.png");
	this->win = win;
}

void nico::Button::assignHoverTexture(Texture* hover)
{
	if (hover != nullptr)
		HoverTexture = hover;
}

void nico::Button::assignClickedTexture(Texture* clicked)
{
	if (clicked != nullptr)
		Clickedtexture = clicked;
}

bool nico::Button::isHover()
{
	return (std::abs((win->getHeight() - win->getCursorY()) - this->position.y ) <= this->scale.y * .5) and (std::abs(win->getCursorX() - this->position.x) <= this->scale.x * .5);
}

bool nico::Button::isClicked()
{
	return isHover() and win->Mouse(GLFW_MOUSE_BUTTON_LEFT);
}

void nico::Button::draw(Shader* shader)
{
	preDrawInstructions(shader);

	if (isClicked()) {
		glActiveTexture(GL_TEXTURE0);
		Clickedtexture->bind();

		object->draw(shader);
	}
	else if (isHover()) {
		glActiveTexture(GL_TEXTURE0);
		HoverTexture->bind();

		object->draw(shader);
	}
	else {
		Object2d::draw(shader);
	}
}

nico::Texture* nico::Button::defaultTextures[3];
