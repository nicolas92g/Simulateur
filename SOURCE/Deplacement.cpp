#include "Deplacement.h"

using namespace nico;
using namespace glm;

void deplacement(Physique* montgol, Window* win, std::vector<sphere>* hitboxes) {
	
	montgol->acc = montgol->forces.vent + montgol->forces.g + montgol->forces.archi;
	montgol->vit += montgol->acc * (float)win->getDeltaTime();
	montgol->pos += montgol->vit * (float)win->getDeltaTime();
	
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

void afficheHitbox(sphere* hitbox, Shader* shader){
	static Model sphereModel(NICO_PATH"MODELISATION/hitboxSphere/hitbox.obj");
	static Object3d sphere(&sphereModel);
	
	sphere.setPos(hitbox->centre);
	sphere.setScale(vec3(hitbox->rayon));
	sphere.update(); sphere.draw(shader);
}

void afficheHitbox(std::vector<sphere>* hitbox, Shader* shader, glm::vec3* pos, float Rdistance) {
	for (size_t i = 0; i < hitbox->size(); i++) {
		if(distance(pos, &hitbox->operator[](i).centre) < Rdistance)
			afficheHitbox(&hitbox->operator[](i), shader);
	}
}

std::vector<sphere> sphere::affichage;

bool testDeCollision(std::vector<sphere>* a, std::vector<sphere>* b)
{
	for (size_t i = 0; i < a->size(); i++)
	{
		for (size_t i = 0; i < b->size(); i++)
		{
			if (collision(&(*a)[i], &(*b)[i]))
				return true;
		}
	}
	return false;
}
