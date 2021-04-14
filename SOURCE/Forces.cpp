#include "Forces.h"



glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature) {

	double force;
	double masse = masseVolumiqueAir(temperature) * volume + masseAVide;

	force = masseVolumiqueAir(15) * volume * G;

	if (masse == 0)
		masse = 0.001;

	return vec3(0,force/masse,0);
}

/*
double masseVolumiqueAir(double temperature) {

	double pSat = 611.213 * exp((17.5043 * temperature) / (241, 2 + temperature));

	double Rh = RS / (1 - (HUMIDITE_AIR * pSat / PRESSION) * (1 - RS / RV));

	return PRESSION / (Rh * (temperature + 273.15));
}*/

double masseVolumiqueAir(double temperature) {



	return (PRESSION * M) / (R * (temperature + 273.15));


}

