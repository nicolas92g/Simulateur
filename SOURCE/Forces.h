
#include <nico.hpp>
#include <noise.h>

#define RHO 1.225
#define G 9.81
#define PRESSION 101325.0   //90000.0
#define R 8.3144621
#define M 0.029

#define TEMPERATURE_AMBIANTE 15
#define TEMPERATURE_MAX 130
#define ACTION_DU_TEMPS 1e-4
#define ACTION_DU_ALTITUDE 1e-3
#define ACTION_DU_FROTTEMENT 0.05f
#define AMPLITUDE_DU_VENT 5.0f
#define TEMPERATURE_PERDUE_PAR_MILLISEC 0.0005f
#define VITESSE_DE_CHAUFFAGE 0.01
#define VITESSE_DE_DECHAUFFAGE 1

glm::vec3 pousseeDArchimede(double masseAVide, double volume, double temperature, float z);
double masseVolumiqueAir(double temperature, float z);
double pressionAir(float z);
glm::vec3 ControleGodVent(nico::Window* win, nico::Camera* cam);
glm::vec3 ForceDuVent(float altitude, double temps, float multiTemps);
glm::vec3 ForceDeFrottements(vec3 vitesse);
float ControleTemperature(nico::Window* win, float temperature, float multiTemps);