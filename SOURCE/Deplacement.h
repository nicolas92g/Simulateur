#pragma once
#include <nico.hpp>
#include <config.hpp>

//correspond à des forces sous forme d'accélération
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

//acc en m/s², vit en m/s, pos en m
struct Physique {
	glm::vec3 acc;
	glm::vec3 vit;
	glm::vec3 pos;
	Forces forces;

	Physique() {
		acc = vec3(0);
		vit = vec3(0);
		pos = vec3(0);
	}
};

//sphere pour former une boite de collision
struct sphere {
	glm::vec3 centre;
	float rayon;

	static std::vector<sphere> affichage;
};

//toutes les fonctions qui implemente le deplacement du joueur

void deplacement(Physique*, nico::Window*);
float distance(glm::vec3* a, glm::vec3* b);
bool collision(sphere* a, sphere* b);

//outils de devellopement
void afficheHitbox(sphere* hitbox, nico::Shader* shader);
void afficheHitbox(std::vector<sphere>* hitbox, nico::Shader* shader, glm::vec3* pos, float distance = 10.0f);
