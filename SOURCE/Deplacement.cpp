 #include "Deplacement.h"

using namespace nico;
using namespace glm;

bool deplacement(Physique* montgol, Window* win, std::vector<sphere>* hitboxes, float multiTemps) {
	
	montgol->acc = montgol->forces.vent + montgol->forces.g + montgol->forces.archi + montgol->forces.frottements;
	montgol->vit += montgol->acc * (float)win->getDeltaTime() * multiTemps;
	vec3 futurPos = montgol->pos + montgol->vit * (float)win->getDeltaTime() * multiTemps;

	std::vector<sphere> mongolHitbox(montgol->hitbox.size());

	for (size_t i = 0; i < montgol->hitbox.size(); i++)
	{
		mongolHitbox[i] = { montgol->hitbox[i].centre + futurPos, montgol->hitbox[i].rayon };
	}

	if (!testDeCollision(&mongolHitbox, hitboxes))
		montgol->pos = futurPos;
	else {
		//supprime le deplacement horizontal
		futurPos.x = montgol->pos.x;
		futurPos.z = montgol->pos.z;
		//reteste la collsion uniquement a la verticale
		for (size_t i = 0; i < montgol->hitbox.size(); i++)
		{
			mongolHitbox[i] = { montgol->hitbox[i].centre + futurPos, montgol->hitbox[i].rayon };
		}
		if (!testDeCollision(&mongolHitbox, hitboxes))
			montgol->pos = futurPos;
		else
			montgol->vit = vec3(0);
	}

	//GameOver ?

	if (montgol->pos.y < 1)
		return true;

	return false;
}

//permet de savoir la distance entre 2 point sans faire de copies des ces points (sizeof(vec3) = 12 | sizeof(vec3*) = 8)
float distancePtr(const vec3* a, const vec3* b){
	static float x, y, z;
	 x = a->x - b->x;
	 y = a->y - b->y;
	 z = a->z - b->z;
	return sqrt(x * x + y * y + z * z);
}

//detecte la colision entre 2 spheres
bool collision(const sphere* a, const sphere* b) {
	return distancePtr(&a->centre, &b->centre) <= a->rayon + b->rayon;
}

//permet d'afficher une sphere 
void afficheHitbox(sphere* hitbox, Shader* shader){
	static Model sphereModel(NICO_PATH"MODELISATION/hitboxSphere/hitbox.obj");
	static Object3d sphere(&sphereModel);
	
	sphere.setPos(hitbox->centre);
	sphere.setScale(vec3(hitbox->rayon));
	sphere.update(); sphere.draw(shader);
}

//permet d'afficher une hitbox 
void afficheHitbox(std::vector<sphere>* hitbox, Shader* shader, glm::vec3* pos, float Rdistance) {
	for (size_t i = 0; i < hitbox->size(); i++) {
		if(distancePtr(pos, &hitbox->operator[](i).centre) < Rdistance)
			afficheHitbox(&hitbox->operator[](i), shader);
	}
}

std::vector<sphere> sphere::affichage;

//permet de detecter une collision entre 2 "hitbox" (des tableaux de spheres quoi)
bool testDeCollision(const std::vector<sphere>* a, const std::vector<sphere>* b)
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

//permet d'eviter le plus possible le Z-fighting tout en essayant de garder une distance d'affichage max élevé
void parametrageDeLaCamera(nico::Camera* cam)
{
	if (cam->getPosition().y > 1000) {
		cam->setZFar(10000.0f);
		cam->setZNear(9.0f);
		Controls::zoomMin = 5.4f;

		if(Controls::zoom < Controls::zoomMin)
			Controls::zoom = 5.4f;
	}
	else {
		cam->setZFar(4500.0f);
		cam->setZNear(0.4f);
		Controls::zoomMin = 1.4f;
	}
	Controls::zoomMax = 20.0f;
}

vec3 animationSoupape(nico::Window* win)
{
	static float hauteurSoupape = 0;
	static constexpr float maxH = 0;
	static constexpr float minH = -0.2;
	static constexpr float speed = .5;

	if (win->Key(GLFW_KEY_LEFT_CONTROL)) {
		hauteurSoupape -=  speed * win->getDeltaTime();
	}
	else { 
		hauteurSoupape += speed * win->getDeltaTime();
	}

	hauteurSoupape = std::max(minH, std::min(maxH, hauteurSoupape));
	return vec3(0, hauteurSoupape, 0);
}

void animationBruleur(nico::Renderer& render, nico::Light& light, const glm::vec3& mongolPos)
{
	static float intensity = 0;
	static constexpr float minI = 0, maxI = 10;
	if (render.Window()->Key(GLFW_KEY_SPACE)) {
		intensity += render.Window()->getDeltaTime() * 20;
	}
	else {
		intensity -= render.Window()->getDeltaTime() * 20;
	}
	
	intensity = std::max(minI, std::min(maxI, intensity));
	
	light.setColor(vec3(2, 0.7, 0.4) * intensity);
	light.setLightAttenuationValues(0.7f);
	
	if (intensity < 0.3f)
		render.removeLightSource(&light);
	else
		render.addLightSource(&light);

	static constexpr float A = .02f;
	light.setPos(mongolPos - vec3((rand() % 10 - 5) * A, 0.2f + (rand() % 10) * A, (rand() % 10 - 5) * A));
}



