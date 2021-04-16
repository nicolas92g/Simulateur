#include "Forces.h"



glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature) {

	double force;
	double masse = masseVolumiqueAir(temperature) * volume + masseAVide;

	force = masseVolumiqueAir(15) * volume * G;

	if (masse == 0)
		masse = 0.001;

	return vec3(0,force/masse,0);
}

glm::vec3 forceDuVent(nico::Window* win) {
	const float speed = .5f;
	glm::vec3 ret = vec3(0);
	if (win->Key(GLFW_KEY_I))
		ret.x += speed;
	if (win->Key(GLFW_KEY_K))
		ret.x -= speed;
	if (win->Key(GLFW_KEY_J))
		ret.z += speed;
	if (win->Key(GLFW_KEY_L))
		ret.z -= speed;
	return ret;
}

double masseVolumiqueAir(double temperature) {



	return (PRESSION * M) / (R * (temperature + 273.15));


}

