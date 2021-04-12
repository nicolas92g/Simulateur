#include "Deplacement.h"

using namespace nico;
using namespace glm;

void deplacement(Physique* montgol, Window* win) {
	
	montgol->acc = montgol->forces.vent + montgol->forces.g + montgol->forces.archi;
	montgol->vit += montgol->acc *(float) win->getDeltaTime();
	montgol->pos += montgol->vit *(float) win->getDeltaTime();
	
}