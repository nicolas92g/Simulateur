#include "Forces.h"

glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature, float z) {

	double force;
	double masse = masseVolumiqueAir(temperature, z) * volume + masseAVide;

	force = masseVolumiqueAir(TEMPERATURE_AMBIANTE, z) * volume * G;

	if (masse == 0)
		masse = 0.001;

	return vec3(0,force/masse,0);
}

glm::vec3 ControleGodVent(nico::Window* win, nico::Camera* cam) {
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

glm::vec3 ForceDuVent(float altitude, double temps, float multiTemps) {
	static int seedAngle = rand();
	static int seedAmplitude = rand();

	noise::module::Perlin noise;
	noise.SetOctaveCount(1);
	noise.SetSeed(seedAngle);
	double angle = noise.GetValue(temps * ACTION_DU_TEMPS * multiTemps, altitude * ACTION_DU_ALTITUDE, 1) * PI;

	vec3 direction = vec3(cos(angle), 0, sin(angle));
	noise.SetSeed(seedAmplitude);
	direction *= noise.GetValue(temps * ACTION_DU_TEMPS * multiTemps, altitude * ACTION_DU_ALTITUDE, 1);
	return direction * AMPLITUDE_DU_VENT;
}

glm::vec3 ForceDeFrottements(vec3 vitesse) {
	vec3 fr = -vitesse;
	float vit = glm::length(vitesse);
	return fr * ACTION_DU_FROTTEMENT * vit;
}


double masseVolumiqueAir(double temperature, float z) {
	return (pressionAir(z) * M) / (R * (temperature + 273.15));
}

double pressionAir(float z) {

	return PRESSION * std::pow(1 - 2.25577e-5 * z, 5.255);
}


float ControleTemperature(nico::Window* win, float temperature, float multiTemps) {
	float temp = temperature - (temperature - TEMPERATURE_AMBIANTE) * win->getDeltaTime() * TEMPERATURE_PERDUE_PAR_MILLISEC * multiTemps;
	if (win->Key(GLFW_KEY_SPACE)) {
		temp += (TEMPERATURE_MAX - temp) * win->getDeltaTime() * VITESSE_DE_CHAUFFAGE * multiTemps;
	}
	return temp;
}
