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

float distance(vec3* a, vec3* b) {
	static float x, y, z;
	 x = a->x - b->x;
	 y = a->y - b->y;
	 z = a->z - b->z;
	return sqrt(x * x + y * y + z * z);
}

bool collision(sphere* a, sphere* b) {
	return distance(&a->centre, &b->centre) <= a->rayon + b->rayon;
}