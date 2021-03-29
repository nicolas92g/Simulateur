#include "Controls.h"

using namespace nico;

float Controls::zoom = 3.0f;
float Controls::zoomSpeed = 0.2f;
float Controls::zoomMin = 1.0f;
float Controls::zoomMax = 10.0f;


Controls::Controls(nico::Camera* cam, nico::Object3d* character, nico::Window* win, vec3 offset)
{
	glfwSetScrollCallback(win->glfw(), Controls::scrollCallback);
	this->cam = cam;
	this->win = win;
	this->character = character;
	this->angles = glm::vec2(0, PI * 0.5);
	this->mouseSensivity = 0.004f;
	this->offset = offset;
	this->disableMouse = false;
	this->disableMouseKey = false;
	minAndMaxAngles = glm::vec2(0.51, 1.5);
}

void Controls::update()
{

	//disable mouse
	if (win->Key(GLFW_KEY_TAB)) {
		if (!disableMouseKey) {
			disableMouse = !disableMouse;
			glfwSetCursorPos(win->glfw(), 0, 0);
		}
	}
	disableMouseKey = win->Key(GLFW_KEY_TAB);
	//mouse control
	double mouseX, mouseY;

	if (!disableMouse) {
		glfwGetCursorPos(win->glfw(), &mouseX, &mouseY);
		angles.x += float(mouseSensivity * float(win->getWidth() / 2 - mouseX));
		angles.y += float(mouseSensivity * float(win->getHeight() / 2 - mouseY));

		if (angles.y > minAndMaxAngles.y * PI) {
			angles.y = minAndMaxAngles.y * PI;
		}
		if (angles.y < minAndMaxAngles.x * PI) {
			angles.y = minAndMaxAngles.x * PI;
		}

		glfwSetCursorPos(win->glfw(), (float)win->getWidth() / 2.0f, (float)win->getHeight() / 2.0f);
		glfwSetInputMode(win->glfw(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
	}
	else {
		glfwSetInputMode(win->glfw(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}


	glm::vec3 cameraPos = glm::vec3(1, 0, 0);
	cameraPos = glm::rotate(cameraPos, angles.x, glm::vec3(0, 1, 0));

	cameraPos += offset;

	cameraPos = glm::rotate(cameraPos, angles.y, glm::cross(vec3(0, 1, 0), cameraPos));

	cam->setPosition(character->getPos() + cameraPos * zoom + offset);


	cam->setLook(glm::normalize(character->getPos() - cam->getPosition() + offset));
	cam->calculateMatrix(win);
}

void Controls::scrollCallback(GLFWwindow* win, double x, double y)
{
	zoom -= y * zoomSpeed;

	if (zoom < zoomMin) {
		zoom = zoomMin;
	}
	if (zoom > zoomMax) {
		zoom = zoomMax;
	}
}
