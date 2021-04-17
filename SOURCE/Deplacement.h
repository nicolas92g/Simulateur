#pragma once
#include <nico.hpp>
#include <config.hpp>
#include "Controls.h"

//correspond � des forces sous forme d'acc�l�ration
struct Forces {
	glm::vec3 archi;
	glm::vec3 g;
	glm::vec3 vent;

	Forces() {
		archi = vec3(0);
		g = vec3(0, -9.81, 0);
		vent = vec3(0);
	}
};

//sphere pour former une boite de collision
struct sphere {
	glm::vec3 centre;
	float rayon;

	static std::vector<sphere> affichage;
};

//acc en m/s�, vit en m/s, pos en m
struct Physique {
	glm::vec3 acc;
	glm::vec3 vit;
	glm::vec3 pos;
	Forces forces;
	std::vector<sphere> hitbox;

	Physique() {
		acc = vec3(0);
		vit = vec3(0);
		pos = vec3(0);
	}
};



//toutes les fonctions qui implemente le deplacement du joueur

void deplacement(Physique*, nico::Window*, std::vector<sphere>* hitboxes);
float distance(glm::vec3* a, glm::vec3* b);
bool collision(sphere* a, sphere* b);
bool testDeCollision(std::vector<sphere>* a, std::vector<sphere>* b);
void parametrageDeLaCamera(nico::Camera* cam);

//outils de devellopement
void afficheHitbox(sphere* hitbox, nico::Shader* shader);
void afficheHitbox(std::vector<sphere>* hitbox, nico::Shader* shader, glm::vec3* pos, float distance = 10.0f);
