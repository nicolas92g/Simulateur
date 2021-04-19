#include "Forces.h"
#define ACTION_DU_TEMPS 0.001
#define ACTION_DU_ALTITUDE 0.01
#define ACTION_DU_FROTTEMENT 0.2f
#define AMPLITUDE_DU_VENT 5.0f
#define TEMPERATURE_PERDUE_PAR_MILLISEC 0.002f
#define VITESSE_DE_CHAUFFAGE 0.01

glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature) {

	double force;
	double masse = masseVolumiqueAir(temperature) * volume + masseAVide;

	force = masseVolumiqueAir(TEMPERATURE_AMBIANTE) * volume * G;

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

glm::vec3 ForceDuVent(float altitude, double temps) {
	static int seedAngle = rand();
	static int seedAmplitude = rand();

	noise::module::Perlin noise;
	noise.SetOctaveCount(1);
	noise.SetSeed(seedAngle);
	double angle = noise.GetValue(temps * ACTION_DU_TEMPS, altitude * ACTION_DU_ALTITUDE, 1) * PI;

	vec3 direction = vec3(cos(angle), 0, sin(angle));
	noise.SetSeed(seedAmplitude);
	direction *= noise.GetValue(temps * ACTION_DU_TEMPS, altitude * ACTION_DU_ALTITUDE, 1);
	return direction * AMPLITUDE_DU_VENT;
}

glm::vec3 ForceDeFrottements (vec3 vitesse) {
	vec3 fr = -vitesse;
	float vit = glm::length(vitesse);
	return fr * ACTION_DU_FROTTEMENT * vit;
}


double masseVolumiqueAir(double temperature) {



	return (PRESSION * M) / (R * (temperature + 273.15));


}

float ControleTemperature(nico::Window* win, float temperature) {
	float temp = temperature - (temperature - TEMPERATURE_AMBIANTE) * win->getDeltaTime() * TEMPERATURE_PERDUE_PAR_MILLISEC;
	if (win->Key(GLFW_KEY_SPACE)) {
		temp += (TEMPERATURE_MAX - temp) * win->getDeltaTime() * VITESSE_DE_CHAUFFAGE;
	}
	return temp;
}
