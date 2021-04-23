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

GameOverInterface::GameOverInterface(nico::Window* win, nico::TextRenderer* text):background(NICO_TEXTURES_PATH"white.png"), Object2d(&background), 
recommencer(win), reprendre(win), quitter(win)
{
	this->win = win;
	this->text = text;
	transparency = 0.5;
	mixedColor = glm::vec3(0);

	state = State::playing;

}

void GameOverInterface::draw(nico::Shader* shader)
{
	if (state == State::playing)
		return;

	const float x = win->getWidth() * .5;
	const float y = win->getHeight() * .5;

	position = glm::vec2(x, y);
	scale = glm::vec2(win->getWidth() * .7, y * 2);
	Object2d::draw(shader);

	text->print("GAME OVER", x - text->calculateTextRenderingSize("GAME OVER", y * .2 ).x * .5, y * 1.5, y * .2, glm::vec4(1), false);

	recommencer.setPosition(glm::vec2(x, y));
	recommencer.setScale(glm::vec2(x * .5, x * .1));
	recommencer.draw(shader);
	text->print("Recommencer", x - text->calculateTextRenderingSize("Recommencer", y * .075).x * .5, y, y * .075, glm::vec4(0,0,0,1), false);

	reprendre.setPosition(glm::vec2(x, y * .7));
	reprendre.setScale(glm::vec2(x * .5, x * .1));
	reprendre.draw(shader);
	text->print("Reprendre", x - text->calculateTextRenderingSize("Reprendre", y * .075).x * .5, y * .7, y * .075, glm::vec4(0, 0, 0, 1), false);

	quitter.setPosition(glm::vec2(x, y * .4));
	quitter.setScale(glm::vec2(x * .5, x * .1));
	quitter.draw(shader);
	text->print("Quitter", x - text->calculateTextRenderingSize("Quitter", y * .075).x * .5, y * .4, y * .075, glm::vec4(0, 0, 0, 1), false);

	if (recommencer.isClicked())
		state = State::recommencer;

	if (reprendre.isClicked())
		state = State::reprendre;

	if (quitter.isClicked())
		win->setShouldClose(true);
}

void GameOverInterface::setState(State state)
{
	this->state = state;
}

GameOverInterface::State GameOverInterface::getState() const
{
	return state;
}
