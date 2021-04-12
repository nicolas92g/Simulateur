#include <nico.hpp>
#include <config.hpp>

//correspond à des forces sous forme d'accélération
struct Forces {  
	glm::vec3 archi;
	glm::vec3 g;
	glm::vec3 vent;
};

//acc en m/s² , vit en m/s , pos en m
struct Physique {
	glm::vec3 acc;
	glm::vec3 vit;
	glm::vec3 pos;
	Forces forces;
};

void deplacement(Physique*, nico::Window*);