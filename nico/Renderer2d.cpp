#include "Renderer2d.h"


using namespace glm;


nico::Renderer2d::Renderer2d(nico::Window* win) :
	shader(NICO_SHADERS_PATH"renderer2d.vert", NICO_SHADERS_PATH"renderer2d.frag")
{
	this->win = win;
	Object2d::CreatePlane();
}

nico::Renderer2d::~Renderer2d()
{
	Object2d::DestroyPlane();
	Button::destroyDefaultTextures();
}

void nico::Renderer2d::addElement(Object2d* object)
{
	for (std::list<Object2d*>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (object == (*it)) {
			return;
		}
	}
	objects.push_back(object);
}

void nico::Renderer2d::removeElement(Object2d* object)
{
	for (std::list<Object2d*>::iterator it = objects.begin(); it != objects.end(); it++) {
		if (object == (*it)) {
			objects.erase(it);
		}
	}
}

void nico::Renderer2d::assignWindow(nico::Window* win)
{
	if (win != nullptr)
		this->win = win;
}

void nico::Renderer2d::frame()
{
	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glViewport(0,0, win->getWidth(), win->getHeight());

	shader.set("projection", glm::ortho<float>(0, win->getWidth(), 0, win->getHeight()));

	for (std::list<Object2d*>::iterator it = objects.begin(); it != objects.end(); it++) {
		(**it).draw(&shader);
	}
}

void nico::Renderer2d::updateLayers()
{
	objects.sort([](Object2d* a, Object2d* b){
		return *a < *b;
	});
}

nico::Plane::Plane()
{
	//fill vertices
	std::vector<float> vertices = {//this is a square that is all over the screen
		// positions	// texture Coords
		-0.5f,  0.5f,	0.0f, 1.0f,
		-0.5f, -0.5f,	0.0f, 0.0f,
		 0.5f,  0.5f,	1.0f, 1.0f,
		 0.5f, -0.5f,	1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	VertexBuffer<float> VBO(&vertices, 4);
	VBO.addLayout(0, 4);
}

void nico::Plane::draw(Shader* shader)
{
	glBindVertexArray(VAO);
	shader->use();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}
