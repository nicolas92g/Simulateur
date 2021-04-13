#include <SOURCE/Header.h>
#include <nico.hpp>
#include <config.hpp>
#include <TERRAIN/Land.h>
#include "Controls.h"
#include "Deplacement.h"
#include "Forces.h"

using namespace nico;
using namespace glm;


int main() {

	//initialisation
	Renderer render;
	Camera player;
	render.useCamera(&player);
	player.setZFar(4000);
	Renderer2d render2d(render.Window());

	NumberInput masse(render.Window());
	render2d.addElement(&masse);
	masse.setPosition(vec2(200, 200));
	masse.setValue(50);

	NumberInput volume(render.Window());
	render2d.addElement(&volume);
	volume.setPosition(vec2(200, 100));
	volume.setValue(1400);

	NumberInput temperature(render.Window());
	render2d.addElement(&temperature);
	temperature.setPosition(vec2(200, 300));
	temperature.setValue(80);

	//Creation de la Montgolfière
	Model montgolGeo(NICO_PATH"MODELISATION/baloon.obj");
	Object3d montgol(&montgolGeo);
	render.addEntity(&montgol);

	//creation de la physique qui gerera la position de la montgolfiere
	Physique montgolPhysique;
	montgolPhysique.vit = vec3(0, 200, -30);
	montgolPhysique.pos = vec3(-458, 30, 3470);
	
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

	do {
		//nettoie l'image
		render.clear();

		montgolPhysique.forces.archi = pousseeDArchimede(masse.getValue(),volume.getValue(),temperature.getValue());

		println(montgolPhysique.forces.archi);

		//fonction qui gere la physique de deplacement 
		deplacement(&montgolPhysique, render.Window());
		montgol.setPos(montgolPhysique.pos);

		//met a jour la camera avec la souris
		souris.update();

		//affiche tous les Object3d
		render.update();
		render.frame();

		//affiche l'image d'arriere plan
		render.drawEnvironmentMapAsSkyMap();

		//affiche le terrain
		terrain.update();
		terrain.draw();

		//2d
		render2d.frame();
				
	} while (!render.Window()->shouldClose());//ferme la fenetre

	while (Chunk::getNumberOfWorkingThreads());//attends que tous les programmes parallele soit terminé

	return 0;
}


