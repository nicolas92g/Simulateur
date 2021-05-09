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

Interface2d::Interface2d(nico::Window* win, nico::TextRenderer* text) : background(NICO_TEXTURES_PATH"white.png"), Object2d(&background), 
recommencer(win), reprendre(win), quitter(win), reprendrePlusHaut(win), 
thermometerTexture(NICO_TEXTURES_PATH"thermometre.png"), thermometre(&thermometerTexture),
variometerTexture(NICO_TEXTURES_PATH"variometre.png"), variometre(&variometerTexture),
altimeterTexture(NICO_TEXTURES_PATH"altimetre.png"), altimeter(&altimeterTexture),
sevenSegment(NICO_TEXTURES_PATH"seven.ttf"),
helpTexture(NICO_TEXTURES_PATH"help.png"), helpHoverTexture(NICO_TEXTURES_PATH"helpHover.png"), helpClickedTexture(NICO_TEXTURES_PATH"helpClicked.png"),
help(win, &helpTexture), retour(win)
{
	this->win = win;
	this->text = text;
	transparency = 0.5;
	mixedColor = glm::vec3(0);

	state = State::playing;
	temperature = 15.0f;
	accelerationVerticale = .0f;
	altitude = 0;
	
	altimeter.setTransparency(.9f);
	variometre.setTransparency(.9f);
	thermometre.setTransparency(.9f);

	help.assignHoverTexture(&helpHoverTexture);
	help.assignClickedTexture(&helpClickedTexture);
	timeAcceleration = 1.0f;
}

void Interface2d::draw(nico::Shader* shader)
{
	const float x = win->getWidth() * .5;
	const float y = win->getHeight() * .5;

	if (state == State::playing) {
		if (win->Key(GLFW_KEY_ESCAPE)) {
			state = State::pause;
			win->hideCursor(false);
		}

		static bool left = false;
		if (win->Key(GLFW_KEY_LEFT) and !left)
			decreaseTimeAcceleration();

		left = win->Key(GLFW_KEY_LEFT);
		
		static bool right = false;
		if (win->Key(GLFW_KEY_RIGHT) and !right)
			increaseTimeAcceleration();

		right = win->Key(GLFW_KEY_RIGHT);

		if (timeAcceleration > 1.0f) {
			static std::string t;
			t = "temps x" + std::to_string((int)timeAcceleration);
			if (timeAcceleration - (float)(int)timeAcceleration > 0.35f) {
				t.append(".5");
			}
			text->print(t, x * 1.95f - text->calculateTextRenderingSize(t, y * .07f).x, y * 0.05, y * .07f);
		}
		
		std::string thermometerText;
		if (temperature < 100) {
			thermometerText = std::to_string((int)(temperature * 10));
			thermometerText.insert(2, ".");
		}
		else {
			thermometerText = std::to_string((int)(temperature));
		}
		std::string variometerText = std::to_string(abs(accelerationVerticale));
		variometerText = variometerText.substr(0, 4);
		if (accelerationVerticale < 0) variometerText.insert(0, "-");

		thermometre.setScale(glm::vec2(sevenSegment.calculateTextRenderingSize("88.8  C", y * 0.12).x, y * 0.12));
		thermometre.setPosition(glm::vec2(x * 0.05  + thermometre.getScale().x * .45, y * 0.2 + thermometre.getScale().y * .25));

		thermometre.draw(shader);
		sevenSegment.print(thermometerText, x * 0.05, y * 0.2, y * 0.1, glm::vec4(0.9, 0.04, 0.02, 1), false);

		variometre.setScale(thermometre.getScale());
		variometre.setPosition(thermometre.getPosition() - thermometre.getScale() * glm::vec2(0, 1.0f));
		
		variometre.draw(shader);
		sevenSegment.print(variometerText, variometre.getPosition().x - thermometre.getScale().x * .45,
			variometre.getPosition().y - thermometre.getScale().y * .25, y * 0.1, glm::vec4(0.9, 0.04, 0.02, 1), false);

		altimeter.setScale(thermometre.getScale());
		altimeter.setPosition(thermometre.getPosition() + thermometre.getScale() * glm::vec2(0, 1.0f));

		altimeter.draw(shader);
		sevenSegment.print(std::to_string((int)altitude), variometre.getPosition().x - thermometre.getScale().x * .45,
			altimeter.getPosition().y - thermometre.getScale().y * .2, y * 0.1, glm::vec4(0.9, 0.04, 0.02, 1), false);

		return;
	}
	win->Key(GLFW_KEY_ESCAPE);//ca ca regle un bug me demande pas pourquoi frerot

	if (state == State::reprendre) return;
	if (state == State::recommencer) return;
	if (state == State::recommencerAuMemeEndroit) return;
		

	position = glm::vec2(x, y);
	scale = glm::vec2(win->getWidth() * .7, y * 2);
	Object2d::draw(shader);

	if (state == State::gameOver) {

		text->print("GAME OVER", x - text->calculateTextRenderingSize("GAME OVER", y * .2).x * .5, y * 1.5, y * .2, glm::vec4(1), false);

		recommencer.setPosition(glm::vec2(x, y));
		recommencer.setScale(glm::vec2(x * .5, x * .1));
		recommencer.draw(shader);
		text->print("Recommencer", x - text->calculateTextRenderingSize("Recommencer", y * .075).x * .5, y, y * .075, glm::vec4(0, 0, 0, 1), false);

		reprendrePlusHaut.setPosition(glm::vec2(x, y * .7));
		reprendrePlusHaut.setScale(glm::vec2(x * .5, x * .1));
		reprendrePlusHaut.draw(shader);
		text->print("Reprendre", x - text->calculateTextRenderingSize("Reprendre", y * .075).x * .5, y * .7, y * .075, glm::vec4(0, 0, 0, 1), false);

		quitter.setPosition(glm::vec2(x, y * .4));
		quitter.setScale(glm::vec2(x * .5, x * .1));
		quitter.draw(shader);
		text->print("Quitter", x - text->calculateTextRenderingSize("Quitter", y * .075).x * .5, y * .4, y * .075, glm::vec4(0, 0, 0, 1), false);

		if (recommencer.isClicked())
			state = State::recommencer;

		else if (reprendrePlusHaut.isClicked())
			state = State::recommencerAuMemeEndroit;

		else if (quitter.isClicked())
			win->setShouldClose(true);
	}

	else if (state == State::pause) {
		text->print("PAUSE", x - text->calculateTextRenderingSize("PAUSE", y * .2).x * .5, y * 1.5, y * .2, glm::vec4(1), false);

		recommencer.setPosition(glm::vec2(x, y));
		recommencer.setScale(glm::vec2(x * .5, x * .1));
		recommencer.draw(shader);
		text->print("Recommencer", x - text->calculateTextRenderingSize("Recommencer", y * .075).x * .5, y, y * .075, glm::vec4(0, 0, 0, 1), false);

		reprendre.setPosition(glm::vec2(x, y * .7));
		reprendre.setScale(glm::vec2(x * .5, x * .1));
		reprendre.draw(shader);
		text->print("Reprendre", x - text->calculateTextRenderingSize("Reprendre", y * .075).x * .5, y * .7, y * .075, glm::vec4(0, 0, 0, 1), false);

		quitter.setPosition(glm::vec2(x, y * .4));
		quitter.setScale(glm::vec2(x * .5, x * .1));
		quitter.draw(shader);
		text->print("Quitter", x - text->calculateTextRenderingSize("Quitter", y * .075).x * .5, y * .4, y * .075, glm::vec4(0, 0, 0, 1), false);

#		ifdef VERSION_DE_CLEMENT
		text->print("connard et testeur legendaire : Clement Guillot", x - text->calculateTextRenderingSize("connard et testeur légendaire : Clement Guillot", y * .045).x * .5, y * .03, y * .045, glm::vec4(0.5, 0, 0, 1), false);
#		endif

		help.setPosition(glm::vec2(x * 0.8, y * 1.3));
		help.setScale(glm::vec2(x * 0.05, x * 0.05));
		help.draw(shader);
		
		if (recommencer.isClicked())
			state = State::recommencer;

		else if (reprendre.isClicked())
			state = State::reprendre;

		else if (quitter.isClicked())
			win->setShouldClose(true);

		else if (help.isClicked())
			state = State::help;
	}

	else if (state == State::help) {
		static const char const* title = "Comment utiliser ce (merveilleux) simulateur ?";
		text->print(title, x - text->calculateTextRenderingSize(title, y * .08).x * .5, y * 1.7, y * .08, glm::vec4(1), false);
		
		static const char const* line1 = 
			"Tout d'abord, vous pouvez controler l'altitude en utilisant espace pour chauffer le ballon et control gauche pour ouvrir la soupape.";
		text->print(line1, x - text->calculateTextRenderingSize(line1, y * .04).x * .5, y * 1.5, y * .04, glm::vec4(1), false);
		
		static const char const* line2 = 
			"Ensuite, les deplacements lateraux n'ont pas de touches mais vous pouvez regarder en haut de l'ecran ou des directions sont indiquees";
		text->print(line2, x - text->calculateTextRenderingSize(line2, y * .04).x * .5, y * 1.4, y * .04, glm::vec4(1), false);
		
		static const char const* line3 = 
			"comme celle du vent ce qui peut vous permetre de choisir une altitude ou le vent vous convient.";
		text->print(line3, x - text->calculateTextRenderingSize(line3, y * .04).x * .5, y * 1.3, y * .04, glm::vec4(1), false);

		static const char const* line4 =
			"De plus, en bas a gauche, vous pourez apercevoir un altimetre, un thermometre et un accelerometre qui vous permetra de savoir";
		text->print(line4, x - text->calculateTextRenderingSize(line4, y * .04).x * .5, y * 1.2, y * .04, glm::vec4(1), false);

		static const char const* line5 =
			"respectivement l'altitude du ballon, sa temperature interne, et son acceleration verticale. Pour finir, vous pouvez utiliser";
		text->print(line5, x - text->calculateTextRenderingSize(line5, y * .04).x * .5, y * 1.1, y * .04, glm::vec4(1), false);

		static const char const* line6 =
			"la molette pour zoomer/dezoomer, F11 pour activer/desactiver le pleine ecran et F3 pour afficher quelques infos en plus";
		text->print(line6, x - text->calculateTextRenderingSize(line6, y * .04).x * .5, y * 1.0, y * .04, glm::vec4(1), false);

		static const char const* line7 =
			"PS : a oui j'allais oublier ! la montgolfiere c hyper chiant ducoup bas vous pouvez accelerer le temps avec les fleches ";
		text->print(line7, x - text->calculateTextRenderingSize(line7, y * .04).x * .5, y * 0.9, y * .04, glm::vec4(1), false);


		retour.setPosition(glm::vec2(x, y * .6));
		retour.setScale(glm::vec2(x * .5, x * .1));
		retour.draw(shader);
		text->print("retour", x - text->calculateTextRenderingSize("retour", y * .075).x * .5, y * .6, y * .075, glm::vec4(0, 0, 0, 1), false);

		if (retour.isClicked())
			state = State::pause;
	}
}

void Interface2d::setState(State state)
{
	this->state = state;
}

Interface2d::State Interface2d::getState() const
{
	return state;
}

void Interface2d::setThermometer(const float temperature)
{
	this->temperature = temperature;
}

void Interface2d::setVariometer(const float accelerationVerticale)
{
	this->accelerationVerticale = accelerationVerticale;
}

void Interface2d::setAltimeter(const float altitude)
{
	this->altitude = altitude;
}

void Interface2d::increaseTimeAcceleration()
{
	timeAcceleration += 0.5f;
	if (timeAcceleration > 10.0f) timeAcceleration = 10.0f;
}

void Interface2d::decreaseTimeAcceleration()
{
	timeAcceleration -= 0.5f;
	if (timeAcceleration < 1) timeAcceleration = 1;
}

const float Interface2d::getTimeAcceleration() const
{
	return timeAcceleration;
}
