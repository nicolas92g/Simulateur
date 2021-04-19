
#include <nico.hpp>
#include <noise.h>

#define RHO 1.225
#define G 9.81
#define PRESSION 101325.0   //90000.0
#define R 8.3144621
#define M 0.029

#define RS 287.06
#define RV 461.0
#define HUMIDITE_AIR 0.76

#define TEMPERATURE_AMBIANTE 15
#define TEMPERATURE_MAX 130
#define ACTION_DU_TEMPS 0.001
#define ACTION_DU_ALTITUDE 0.01
#define ACTION_DU_FROTTEMENT 0.05f
#define AMPLITUDE_DU_VENT 5.0f
#define TEMPERATURE_PERDUE_PAR_MILLISEC 0.002f
#define VITESSE_DE_CHAUFFAGE 0.01

glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature);
double masseVolumiqueAir(double temperature);
glm::vec3 ControleGodVent(nico::Window* win, nico::Camera* cam);
glm::vec3 ForceDuVent(float altitude, double temps);
glm::vec3 ForceDeFrottements(vec3 vitesse);
float ControleTemperature(nico::Window* win, float temperature);