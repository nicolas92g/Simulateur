#include <nico.hpp>

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
		gameOver, playing, recommencer, recommencerAuMemeEndroit, pause, reprendre 
	};

	void setState(State state);
	State getState() const;



private:
	nico::Texture background;
	nico::Window* win;
	nico::TextRenderer* text;

	nico::Button recommencer;
	nico::Button reprendrePlusHaut;
	nico::Button quitter;

	nico::Button reprendre;

	State state;


};