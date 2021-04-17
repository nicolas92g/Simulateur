#include "Forces.h"



glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature) {

	double force;
	double masse = masseVolumiqueAir(temperature) * volume + masseAVide;

	force = masseVolumiqueAir(15) * volume * G;

	if (masse == 0)
		masse = 0.001;

	return vec3(0,force/masse,0);
}

glm::vec3 forceDuVent(nico::Window* win, nico::Camera* cam) {
	const float speed = 4.5f;
	glm::vec3 ret = vec3(0);
	if (win->Key(GLFW_KEY_I))
		ret += glm::normalize(cam->getLook() * vec3(1, 0, 1)) * speed ;
	if (win->Key(GLFW_KEY_K))
		ret += glm::normalize(cam->getLook() * vec3(1, 0, 1)) * -speed ;
	if (win->Key(GLFW_KEY_J))
		ret += cam->getRightDirection() * -speed;
	if (win->Key(GLFW_KEY_L))
		ret += cam->getRightDirection() * speed;
	return ret;
}

double masseVolumiqueAir(double temperature) {



	return (PRESSION * M) / (R * (temperature + 273.15));


}

