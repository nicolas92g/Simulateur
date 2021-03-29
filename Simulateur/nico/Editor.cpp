#include "Editor.h" 
#define ANIMATION_SPEED 5
#ifdef NICO_RENDERING_DEBUG

using namespace glm;

nico::Editor::Editor(Window* win) : background(win, new Texture(NICO_TEXTURES_PATH"white.png")), Object2d(nullptr)
{
	background.setTransparency(0.5f);
	background.setMultiplyColor(glm::vec3(1, 1, 0.7));

	this->win = win;
	this->relativeWidth = .5f;
	this->realRelativeWidth = .0f;
}

void nico::Editor::draw(Shader* shader)
{

	updateRelativeWidth();
	background.setScale(vec2(win->getWidth() * this->realRelativeWidth, win->getHeight()));
	background.setPosition(vec2(win->getWidth() - background.getScale().x * .5, win->getHeight() * .5));

	int y = 500;



	background.draw(shader);
}

void nico::Editor::setRelativeWidth(float Rwidth)
{
	this->relativeWidth = std::max<float>(std::min<float>(Rwidth, 1), 0);
}

void nico::Editor::updateRelativeWidth()
{
	if (std::abs(realRelativeWidth - relativeWidth) < 0.001) {
		realRelativeWidth = relativeWidth;
	}
	else {
		realRelativeWidth += ((size_t)relativeWidth - (size_t)realRelativeWidth) * ANIMATION_SPEED * win->getDeltaTime();
		
	}
}

float* nico::Editor::getVecFromJson(Json::Value root)
{
	float* ret;

	ret = new float[root.size()];

	for (auto it = root.begin(); it != root.end(); it++) {
		if (it->isConvertibleTo(Json::ValueType::realValue)) {
			if (it.name() == "x" or it.name() == "r") {
				ret[0] = it->asFloat();
			}
			if (it.name() == "y" or it.name() == "g") {
				ret[1] = it->asFloat();
			}
			if (it.name() == "z" or it.name() == "b") {
				ret[2] = it->asFloat();
			}
			if (it.name() == "w" or it.name() == "a") {
				ret[3] = it->asFloat();
			}
		}
		else {
			std::cout << "\n issue while reading a vector from a json\n";
			return nullptr;
		}
	}
	return ret;
}


#endif // NICO_RENDERING_DEBUG