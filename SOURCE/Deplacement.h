#pragma once
#include <nico.hpp>
#include <config.hpp>
#include "Controls.h"
#include "Forces.h"

//correspond à des forces sous forme d'accélération
struct Forces {
	glm::vec3 archi;
	glm::vec3 g;
	glm::vec3 vent;
	glm::vec3 frottements;

	Forces() {
		archi = vec3(0);
		g = vec3(0, -9.81, 0);
		vent = vec3(0);
		frottements = vec3(0);
	}
};

//sphere pour former une boite de collision
struct sphere {
	glm::vec3 centre;
	float rayon;

	static std::vector<sphere> affichage;
};

//acc en m/s², vit en m/s, pos en m
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

bool deplacement(Physique*, nico::Window*, std::vector<sphere>* hitboxes, float multiTemps);
float distancePtr(const glm::vec3* a, const glm::vec3* b);
bool collision(const sphere* a, const sphere* b);
bool testDeCollision(const std::vector<sphere>* a, const std::vector<sphere>* b);
void parametrageDeLaCamera(nico::Camera* cam);
vec3 soupapeAnimation(nico::Window* win);
void animationBruleur(nico::Window* win, nico::Light& light, const glm::vec3& mongolPos);

//outils de devellopement
void afficheHitbox(sphere* hitbox, nico::Shader* shader);
void afficheHitbox(std::vector<sphere>* hitbox, nico::Shader* shader, glm::vec3* pos, float distance = 10.0f);
