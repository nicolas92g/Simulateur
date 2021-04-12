#include "Deplacement.h"

using namespace nico;
using namespace glm;

void deplacement(Physique* montgol, Window* win) {
	
	montgol->acc = montgol->forces.vent + montgol->forces.g + montgol->forces.archi;
	montgol->vit += montgol->acc * (float)win->getDeltaTime();
	montgol->pos += montgol->vit * (float)win->getDeltaTime();

	if (montgol->pos.y < 1) {
		montgol->pos.y = 1;
		montgol->vit.y = 0.9 * abs(montgol->vit.y);
	}
	
}