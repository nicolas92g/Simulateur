#include "Deplacement.h"

using namespace nico;
using namespace glm;

void deplacement(Physique* montgol, Window* win, std::vector<sphere>* hitboxes) {
	
	montgol->acc = montgol->forces.vent + montgol->forces.g + montgol->forces.archi + montgol->forces.frottements;
	montgol->vit += montgol->acc * (float)win->getDeltaTime();
	vec3 futurPos = montgol->pos + montgol->vit * (float)win->getDeltaTime();

	std::vector<sphere> mongolHitbox(montgol->hitbox.size());

	for (size_t i = 0; i < montgol->hitbox.size(); i++)
	{
		mongolHitbox[i] = { montgol->hitbox[i].centre + futurPos, montgol->hitbox[i].rayon };
	}

	if (!testDeCollision(&mongolHitbox, hitboxes))
		montgol->pos = futurPos;
	else {
		montgol->vit = vec3(0);
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
		for (size_t j = 0; j < b->size(); j++)
		{
			if (collision(&(*a)[i], &(*b)[j]))
				return true;
		}
	}
	return false;
}

void parametrageDeLaCamera(nico::Camera* cam)
{
	if (cam->getPosition().y > 1000) {
		cam->setZFar(10000.0f);
		cam->setZNear(10.0f);
		Controls::zoomMin = 5.4f;

		if(Controls::zoom < Controls::zoomMin)
			Controls::zoom = 5.4f;
	}
	else {
		cam->setZFar(4000.0f);
		cam->setZNear(0.4f);
		Controls::zoomMin = 1.4f;
	}
	Controls::zoomMax = 20.0f;
}
