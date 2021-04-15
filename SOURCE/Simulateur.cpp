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
	TextRenderer text;
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
	temperature.setValue(23.66f);

	//Creation de la Montgolfière
	Model montgolGeo(NICO_PATH"MODELISATION/baloon.obj");
	Object3d montgol(&montgolGeo);
	render.addEntity(&montgol);

	//Creation de la sphere
	Model sphereGeo("C:/Users/nicol/OneDrive/Documents/Graphismes/models/pbrDemo/pbrDemo.obj");

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

	sphere mongolHitbox;
	mongolHitbox.rayon = 2;


	do {
		//nettoie l'image
		render.clear();
		text.updateDisplay(render.Window());

		montgolPhysique.forces.archi = pousseeDArchimede(masse.getValue(),volume.getValue(),temperature.getValue());

		//fonction qui gere la physique de deplacement 
		deplacement(&montgolPhysique, render.Window(), new std::vector<sphere>());
		montgol.setPos(montgolPhysique.pos);
		mongolHitbox.centre = montgolPhysique.pos + vec3(0, 2,0);

		//sphere::affichage.push_back();

		//met a jour la camera avec la souris
		//souris.update();
		player.classicKeyboardControls(render.Window(), 100);
		player.classicMouseControls(render.Window(), 0.004f);

		//affiche l'image d'arriere plan
		render.drawEnvironmentMapAsSkyMap();

		//affiche le terrain
		terrain.update();
		terrain.draw();

		//affiche tous les Object3d
		render.update();
		render.frame();

		//glDepthFunc(GL_ALWAYS);
		afficheHitbox(&sphere::affichage, render.Shader(), &montgolPhysique.pos, 20);
		afficheHitbox(&mongolHitbox, render.Shader());
		sphere::affichage.clear();

		//2d
		render2d.frame();
		text.printLeftTop(nico::strings::vec3Tostring(montgolPhysique.pos));
				
	} while (!render.Window()->shouldClose());//ferme la fenetre

	while (Chunk::getNumberOfWorkingThreads());//attends que tous les programmes parallele soit terminé

	return 0;
}


