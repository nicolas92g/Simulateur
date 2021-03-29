#include "Camera.h"
using namespace glm;

nico::Camera::Camera(vec3 pos, float pitch_, float yaw_, float fov_)
{
	position = pos;
	pitch = pitch_;
	yaw = yaw_;
	fov = fov_;
	zNear = 0.1f;
	zFar = 100.0f;
	upDirection =  vec3(0,1,0);
	//just to initalize vars
	updateLookWithYawAndPitch();
	calculateMatrix(10,10);
}

void nico::Camera::calculateMatrix(Window *win)
{
	//make our class compatible with  nico::Window class
	if (win->getWidth() && win->getHeight())
		calculateMatrix((float)win->getWidth(),(float)win->getHeight());
}

void nico::Camera::calculateMatrix(float width, float height)
{
	//transform pitch and yaw into a look vector
#	ifdef DEBUG
	assert(this != nullptr);// if this line create a debug breakpoint is because you try to modif a camera that does'nt exist
#	endif DEBUG

	//put all vars into 4x4 matrix transformation
	view = lookAt(position, position + look, upDirection);
	projection = perspective(fov, width / height,zNear, zFar);
}

void nico::Camera::updateLookWithYawAndPitch()
{
	look.x = cos(yaw) * cos(pitch);
	look.y = sin(pitch);
	look.z = sin(yaw) * cos(pitch);
}

void nico::Camera::sendToShader(Shader *shader_, const char* projection_, const char* view_, const char* pos_)
{
	shader_->use();
	shader_->set( projection_, projection);
	shader_->set(view_, view);
	shader_->set(pos_, position);
}
void nico::Camera::classicMouseControls(Window* win, double mouseSensivity)
{
	//disable mouse
	static bool disableMouseKey, disableMouse;
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
		setYaw((float)yaw - float(mouseSensivity * float(win->getWidth() / 2 - mouseX)));
		setPitch((float)pitch + float(mouseSensivity * float(win->getHeight() / 2 - mouseY)));

		glfwSetCursorPos(win->glfw(), (float)win->getWidth() / 2.0f, (float)win->getHeight() / 2.0f);
		glfwSetInputMode(win->glfw(), GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
		updateLookWithYawAndPitch();
	}
	else {
		glfwSetInputMode(win->glfw(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
}
void nico::Camera::classicKeyboardControls(Window* win, float speed)
{
	glm::vec3 lookWithoutY = look;
	lookWithoutY.y = 0;

	if (win->Key(GLFW_KEY_W)) {
		position += (float)win->getDeltaTime() * glm::normalize(lookWithoutY) * speed;
	}
	if (win->Key(GLFW_KEY_S)) {
		position -= (float)win->getDeltaTime() * glm::normalize(lookWithoutY) * speed;
	}
	const glm::vec3 up = glm::vec3(0, 1, 0);
	//lateral vector
	glm::vec3 right = glm::normalize(glm::cross(up, look));

	if (win->Key(GLFW_KEY_A)) {
		position += (float)win->getDeltaTime() * glm::normalize(right) * speed;
	}
	if (win->Key(GLFW_KEY_D)) {
		position -= (float)win->getDeltaTime() * glm::normalize(right) * speed;
	}
	if (win->Key(GLFW_KEY_SPACE)) {
		position.y += (float)win->getDeltaTime() * speed;
	}
	if (win->Key(GLFW_KEY_LEFT_CONTROL)) {
		position.y -= (float)win->getDeltaTime() * speed;
	}
}

bool nico::Camera::isVertexInTheFieldOfView(glm::vec3 vertex, float offset)
{
	//circular projection formula:
	//glm::degrees(glm::acos(glm::dot(glm::normalize((this->position - vertex)), glm::normalize(-this->look)))) <= this->fov + offset
	
	vec4 co = projection * view * vec4(vertex, 1);
	vec2 screen = vec2(co.x / co.z, co.y / co.z); //std::cout << co.z << std::endl;
	if (co.z < -0.1f)
		return false;
 	return ((screen.x > (-1 - offset) and screen.x < (1 + offset)) and (screen.y > (-1 - offset) and screen.y < (1 + offset)));	
}

glm::vec3 nico::Camera::getRightDirection() const
{
	return glm::vec3(glm::normalize(-glm::cross(vec3(0.0f,1.0f,0.0f), look)));
}

glm::vec3 nico::Camera::getPosition() const
{
	return position;
}

glm::vec3 nico::Camera::getLook() const
{
	return glm::normalize(look);
}

glm::vec3 nico::Camera::getUpDirection() const
{
	return upDirection;
}

float nico::Camera::getPitch() const
{
	return pitch;
}

float nico::Camera::getYaw() const
{
	return yaw;
}

glm::mat4 nico::Camera::getProjection() const
{
	return projection;
}

glm::mat4 nico::Camera::getView() const
{
	return view;
}

glm::mat4 nico::Camera::getMVP(glm::mat4& modelMatrix) const
{
	return projection * view * modelMatrix;
}

float nico::Camera::getFov() const
{
	return fov;
}

float nico::Camera::getZNear() const
{
	return zNear;
}

float nico::Camera::getZFar() const
{
	return zFar;
}

void nico::Camera::setPosition(glm::vec3 position_)
{
	position = position_;
}

void nico::Camera::setLook(glm::vec3 look_)
{
	look = look_;
}

void nico::Camera::setUpDirection(glm::vec3 up_)
{
	upDirection = glm::normalize(up_);
}

void nico::Camera::setPitch(float pitch_)
{
	if (pitch_ > PI/2.0f) {
		pitch = PI/ 2.0f;
	}
	else if (pitch_ < -PI / 2.0f) {
		pitch = -PI / 2.0f;
	}
	else {
		pitch = pitch_;
	}
}

void nico::Camera::setYaw(float yaw_)
{
	yaw = yaw_;
}

void nico::Camera::setProjection(glm::mat4 projection_)
{
	projection = projection_;
}

void nico::Camera::setView(glm::mat4 view_)
{
	view = view_;
}

void nico::Camera::setFov(float fov_)
{
	if (fov_ < 0.0f) {
		fov = 0;
	}
	else if (fov_ > 180.0f) {
		fov = 180.0f;
	}
	else {
		fov = fov_;
	}
}

void nico::Camera::setZNear(float zNear_)
{
	if (zNear_ >= 0.0f) {
		zNear = zNear_;
	}
}

void nico::Camera::setZFar(float zFar_)
{
	if (zFar_ >= 0.0f) {
		zFar = zFar_;
	}
}
