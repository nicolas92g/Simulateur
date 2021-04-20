#include <nico.hpp>


class Boussole : public nico::Object2d{
public:
	Boussole(nico::Window* win, nico::Camera* cam);
	void draw(nico::Shader* shader);

	static void createTexture();
private:
	nico::Window* win;
	nico::Camera* cam;

	static std::shared_ptr<nico::Texture> image;
};
