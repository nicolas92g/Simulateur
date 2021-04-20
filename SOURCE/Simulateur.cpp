#include <SOURCE/Header.h>
#include <nico.hpp>
#include <config.hpp>
#include <TERRAIN/Land.h>
#include "Controls.h"
#include "Deplacement.h"
#include "Forces.h"

using namespace nico;
using namespace glm;

#define NOMBRE_DE_CHUNK_MIN 4


int main() {
	//initialisation de l'afficheur 3D 
	Renderer render;

	//creer un afficheur de texte avec la police par default
	TextRenderer text;

	//creer la camera
	Camera player;
	render.useCamera(&player);
	player.setZFar(4000);
	player.setPosition(vec3(-458, 10, 3470));

	//creer l'afficheur 2D
	Renderer2d render2d(render.Window());

	//creer un switch (on/off) pour le mode pleine ecran
	KeySwitch fullscreen(render.Window(), GLFW_KEY_F11);

	NumberInput masse(render.Window());
	render2d.addElement(&masse);
	masse.setPosition(vec2(200, 200));
	masse.setValue(200);

	NumberInput volume(render.Window());
	render2d.addElement(&volume);
	volume.setPosition(vec2(200, 100));
	volume.setValue(1400);

	NumberInput temperature(render.Window());
	render2d.addElement(&temperature);
	temperature.setPosition(vec2(200, 300));
	temperature.setValue(23.66f);

	//Creation de la Montgolfière
	Model montgolGeo(NICO_PATH"MODELISATION/baloon.obj");
	Object3d montgol(&montgolGeo);
	render.addEntity(&montgol);

	//creation de la physique qui gerera la position de la montgolfiere
	Physique montgolPhysique;
	montgolPhysique.vit = vec3(0);
	montgolPhysique.pos = vec3(-458, 10, 3470);
	
	//creation du control de la camera avec la souris
	Controls souris(&player, &montgol, render.Window(),vec3(0, 2, 0));
	souris.minAndMaxAngles = vec2(-0.14, 0.84);

	//creation du soleil
	DirectionalLight sun(vec3(-0.5, -0.2, -0.4));
	sun.setColor(vec3(3));
	render.setDirectionalLight(&sun);

	//creation de l'image d'arriere plan
	render.loadEnvironmentMap(NICO_TEXTURES_PATH"sky.hdr");

	//creation du terrain
	Land terrain(&render, &player);
	render.Window()->maximise();
	
	//titre de la fenetre
	render.Window()->setTitle(" Simulateur de montgolfiere ( v-46.3.2 )");

	//creation de la hitbox de la mongolfiere
	sphere mongolHitbox;
	mongolHitbox.rayon = 2;
	mongolHitbox.centre = vec3(0, 2, 0);
	montgolPhysique.hitbox.push_back(mongolHitbox);

	//attends qu'un certain nbr de chunks soit chargé pour commencer a afficher
	while (terrain.getNumberOfLoadedChunks() < NOMBRE_DE_CHUNK_MIN) {
		terrain.update();
	}

	//redemarre le compteur de temps pour eviter d'avoir un enorme delta-T a la premiere image
	glfwSetTime(0.0);

	do {
		//pleine ecran (F11):
		if(fullscreen.justSwitch())
			render.Window()->setFullscreen(fullscreen);

		//nettoie l'image
		render.clear();
		text.updateDisplay(render.Window());

		//gestion de la temperature
		temperature.setValue(ControleTemperature(render.Window(),temperature.getValue()));

		//creation des forces qui s'appliquent sur la montgolfiere
		montgolPhysique.forces.archi = pousseeDArchimede(masse.getValue(),volume.getValue(),temperature.getValue());
		montgolPhysique.forces.vent = ControleGodVent(render.Window(), &player); //ForceDuVent(montgolPhysique.pos.y, glfwGetTime());
		montgolPhysique.forces.frottements = ForceDeFrottements(montgolPhysique.vit);

		//fonction qui gere la physique de deplacement
		deplacement(&montgolPhysique, render.Window(), terrain.getHitbox(montgolPhysique.pos));
		
		//afficher la montgolfiere a la nouvelle position 
		montgol.setPos(montgolPhysique.pos);

		//met a jour la camera avec la souris
		souris.update();
		//player.classicKeyboardControls(render.Window(), 30);
		//player.classicMouseControls(render.Window(), 0.003f);

		//parametre la camera en fonction de l'altitude (sert a eviter le + possible le Z-fighting)
		parametrageDeLaCamera(&player);
		
		//affiche l'image d'arriere plan
		render.drawEnvironmentMapAsSkyMap();

		//affiche le terrain
		terrain.update();
		terrain.draw();

		//affiche tous les Object3d
		render.update();
		render.frame();

		//glDepthFunc(GL_ALWAYS);
		//afficheHitbox(&sphere::affichage, render.Shader(), &montgolPhysique.pos, 20);
		//sphere::affichage.clear();

		//affichage 2d
		render2d.frame();
		text.printLeftTop(nico::strings::ivec3Tostring(montgolPhysique.pos) + " | acc : " + 
			std::to_string((float)((int)((montgolPhysique.forces.archi.y + montgolPhysique.forces.g.y) * 100)) * 0.01f) + " | vit : "
			+ std::to_string(glm::length(montgolPhysique.vit)));
				
	} while (!render.Window()->shouldClose());//ferme la fenetre

	while (Chunk::getNumberOfWorkingThreads());//attends que tous les programmes parallele soit terminé

	return 0;//exit
}


