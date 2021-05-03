#include <nico.hpp>
#include <float.h>

class Boussole : public nico::Object2d{
public:
	Boussole(nico::Window* win, nico::Camera* cam, nico::TextRenderer* text);
	void draw(nico::Shader* shader);

	void setDirection(glm::vec3 dir);
	void setName(const char* name);

	const char* getName() const;
	const glm::vec2& getDirection() const;

	static void createTexture();
private:
	nico::Window* win;
	nico::Camera* cam;
	nico::TextRenderer* text;
	glm::vec2 direction;
	std::string name;

	static std::shared_ptr<nico::Texture> image;
};

class Interface2d : public nico::Object2d {
public:
	Interface2d(nico::Window* win, nico::TextRenderer* text);
	void draw(nico::Shader* shader);

	enum class State
	{
		gameOver, playing, recommencer, recommencerAuMemeEndroit, pause, reprendre, help
	};

	void setState(State state);
	State getState() const;
	void setThermometer(const float temperature);
	void setVariometer(const float accelerationVerticale);
	void setAltimeter(const float altitude);
	void increaseTimeAcceleration();
	void decreaseTimeAcceleration();

	const float getTimeAcceleration() const;


private:
	float temperature;
	float accelerationVerticale;
	float altitude;
	float timeAcceleration;

	nico::Texture thermometerTexture;
	nico::Object2d thermometre;

	nico::Texture variometerTexture;
	nico::Object2d variometre;

	nico::Texture altimeterTexture;
	nico::Object2d altimeter;

	nico::Texture background;
	nico::Window* win;
	nico::TextRenderer* text;
	nico::TextRenderer sevenSegment;

	nico::Button recommencer;
	nico::Button reprendrePlusHaut;
	nico::Button quitter;

	nico::Button reprendre;
	nico::Button retour;

	nico::Button help;
	nico::Texture helpTexture;
	nico::Texture helpHoverTexture;
	nico::Texture helpClickedTexture;

	State state;


};