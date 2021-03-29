#pragma once
#include <nico.hpp>
#include <glm/gtx/rotate_vector.hpp>

struct Controls
{
public:
	/**
	 * @brief bon ca c'est juste une classe dans laquelle j'ai implémenté un controle de la camera plus 
	 * adapté a une montgolfiere avec la souris genre la camera tourne autour d'un objet 3D et on peu 
	 * controler la distance entre la camera et l'objet ( zoom )  avec la molette 
	 */
	Controls(nico::Camera* cam, nico::Object3d* character, nico::Window* win, vec3 offset = vec3(0));

	//faut juste mettre ca dans la boucle
	void update();

	//variable publique parce que flemme :

	//modifie la distance entre la camera et l'objet
	static float zoom;
	//modifie la vitesse du zoom
	static float zoomSpeed;
	//maximun et mininum du zoom
	static float zoomMin;
	static float zoomMax;

	//angle de la camera en radians je crois
	glm::vec2 angles;
	//sensibilité de la souris
	float mouseSensivity;
	//ca c'est pour regler le centre de l'objet a offset = (0, 0) le centre de rotation de la cam est sur le centre geometrique de l'objet
	glm::vec3 offset;
	//les limites en haut et en bas de l'angle y (c'est chelou ca a l'air de changer avec la valeur de : offset)
	glm::vec2 minAndMaxAngles;

protected:
	static void scrollCallback(GLFWwindow* win, double x, double y);

	nico::Object3d* character;
	nico::Window* win;
	nico::Camera* cam;

	bool disableMouseKey, disableMouse;
};

