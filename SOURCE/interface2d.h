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

class GameOverInterface : public nico::Object2d {
public:
	GameOverInterface(nico::Window* win, nico::TextRenderer* text);
	void draw(nico::Shader* shader);

	enum class State
	{
		waiting, playing, recommencer, reprendre
	};

	void setState(State state);
	State getState() const;



private:
	nico::Texture background;
	nico::Window* win;
	nico::TextRenderer* text;

	nico::Button recommencer;
	nico::Button reprendre;
	nico::Button quitter;

	State state;


};