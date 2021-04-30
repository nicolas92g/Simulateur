#include <SOURCE/Header.h>
#include <nico.hpp>
#include <config.hpp>
#include <TERRAIN/Land.h>
#include "Controls.h"
#include "Deplacement.h"
#include "Forces.h"
#include "interface2d.h"

using namespace nico;
using namespace glm;

#define NOMBRE_DE_CHUNK_MIN 4
#define DEFAULT_POS vec3(-458, 3.5 , 3470)
#define DEFAULT_TEMPERATURE 45


int main() {
	//initialisation de l'afficheur 3D
	Renderer render;

	//creer un afficheur de texte avec la police par default
	TextRenderer text;

	//creer la camera
	Camera player;
	render.useCamera(&player);
	player.setZFar(4000);
	player.setPosition(DEFAULT_POS);

	//creer l'afficheur 2D
	Renderer2d render2d(render.Window());

	//creer un switch (on/off) pour le mode pleine ecran
	KeySwitch fullscreen(render.Window(), GLFW_KEY_F11);

	float masse(200.0f);
	float volume(1400.0f);

	NumberInput temperature(render.Window());
	render2d.addElement(&temperature);
	temperature.setPosition(vec2(200, 100));
	temperature.setValue(DEFAULT_TEMPERATURE);

	NumberInput temps(render.Window());
	render2d.addElement(&temps);
	temps.setPosition(vec2(200, 200));
	temps.setValue(1.0);
	temps.setStep(0.001);
	temps.setLimits(0, 20.0);

	Boussole::createTexture();
	Boussole sunBoussole(render.Window(), &player, &text);
	sunBoussole.setName("soleil");
	sunBoussole.setMultiplyColor(vec3(1,1,0));
	render2d.addElement(&sunBoussole);

	Boussole vent(render.Window(), &player, &text);
	vent.setName("direction du vent");
	render2d.addElement(&vent);

	//GameOver and pause Interface
	Interface2d interface(render.Window(), &text);
	render2d.addElement(&interface);
	bool gameOver = false;
	

	//Creation de la Montgolfière
	Model montgolGeo(NICO_PATH"MODELISATION/baloon.obj");
	Object3d montgol(&montgolGeo);
	render.addEntity(&montgol);

	Model soupapeGeo(NICO_PATH"MODELISATION/soupape/soupape.obj");
	Object3d soupape(&soupapeGeo);
	render.addEntity(&soupape);

	//lumiere du bruleur
	Light bruleur;
	render.addLightSource(&bruleur);

	//creation de la physique qui gerera la position de la montgolfiere
	Physique montgolPhysique;
	montgolPhysique.vit = vec3(0);
	montgolPhysique.pos = DEFAULT_POS;
	
	//creation du control de la camera avec la souris
	Controls souris(&player, &montgol, render.Window(),vec3(0, 2, 0));
	souris.minAndMaxAngles = vec2(-0.14, 0.84);

	//creation du soleil
	DirectionalLight sun(vec3(-0.5, -0.2, -0.4));
	sun.setColor(vec3(3));
	render.setDirectionalLight(&sun);
	sunBoussole.setDirection(-sun.getDirection());

	//creation de l'image d'arriere plan
	render.loadEnvironmentMap(NICO_TEXTURES_PATH"sky.hdr");

	//creation du terrain
	Land terrain(&render, &player);
	render.Window()->maximise();
	
	//titre de la fenetre
	render.Window()->setTitle(" Simulateur de montgolfiere ( v-46.3.2 )");

	//creation de la hitbox de la mongolfiere
	sphere mongolHitbox;
	mongolHitbox.rayon = 2.2;
	mongolHitbox.centre = vec3(0, 1.9, 0);
	montgolPhysique.hitbox.push_back(mongolHitbox);

	//attends qu'un certain nbr de chunks soit chargé pour commencer a afficher
	while (terrain.getNumberOfLoadedChunks() < NOMBRE_DE_CHUNK_MIN)
		terrain.update();
	

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
		temperature.setValue(ControleTemperature(render.Window(),temperature.getValue(), temps.getValue()));

		//creation des forces qui s'appliquent sur la montgolfiere
		montgolPhysique.forces.archi = pousseeDArchimede(masse, volume, temperature.getValue(), montgolPhysique.pos.y);
		montgolPhysique.forces.vent = ForceDuVent(montgolPhysique.pos.y, glfwGetTime(), temps.getValue()) + ControleGodVent(render.Window(), &player);
		montgolPhysique.forces.frottements = ForceDeFrottements(montgolPhysique.vit);

		//met a jour la boussole du vent
		vent.setDirection(montgolPhysique.forces.vent);

		//fonction qui gere la physique de deplacement
		
		if (!gameOver) {
			gameOver = deplacement(&montgolPhysique, render.Window(), terrain.getHitbox(montgolPhysique.pos), temps.getValue());

			if (gameOver) {
				interface.setState(Interface2d::State::gameOver);
				render.Window()->hideCursor(false);
			}
		}
		else {
			if (interface.getState() == Interface2d::State::recommencerAuMemeEndroit) {
				gameOver = false;
				montgolPhysique.pos.y = 50;
				montgolPhysique.vit = vec3(0);
				temps = 1;
				interface.setState(Interface2d::State::playing);
			}
		}
		if (interface.getState() == Interface2d::State::recommencer) {
			gameOver = false;
			montgolPhysique.pos = DEFAULT_POS;
			montgolPhysique.vit = vec3(0);
			temperature.setValue(DEFAULT_TEMPERATURE);
			temps = 1;
			interface.setState(Interface2d::State::playing);

			do {
				terrain.update();
			} while (terrain.getNumberOfLoadedChunks() < NOMBRE_DE_CHUNK_MIN);
				
		}
		else if (interface.getState() == Interface2d::State::pause) {
			temps = 0;
		}
		else if (interface.getState() == Interface2d::State::reprendre) {
			temps = 1;
			interface.setState(Interface2d::State::playing);
			render.Window()->setCursorPos(render.Window()->getWidth() * .5, render.Window()->getHeight() * .5);
		}
		if (interface.getState() == Interface2d::State::playing) {
			if(temps < 1.0)
				temps = 1;
		}

			
			
		//afficher la montgolfiere a la nouvelle position 
		montgol.setPos(montgolPhysique.pos);
		soupape.setPos(montgolPhysique.pos + soupapeAnimation(render.Window()));
		animationBruleur(render.Window(), bruleur, montgolPhysique.pos);

		//met a jour la camera avec la souris
		if (interface.getState() == Interface2d::State::playing)
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
		render.setShadows(montgolPhysique.pos, 100.0f);
		render.update();
		render.frame();

		//glDepthFunc(GL_ALWAYS);
		//afficheHitbox(&sphere::affichage, render.Shader(), &montgolPhysique.pos, 20);
		//sphere::affichage.clear();

		//affichage 2d
		render2d.frame();

		//text.printLeftTop(nico::strings::ivec3Tostring(montgolPhysique.pos) + " | acc : " + 
		//	std::to_string((float)((int)((montgolPhysique.forces.archi.y + montgolPhysique.forces.g.y) * 100)) * 0.01f) + " | vit : "
		//	+ std::to_string(glm::length(montgolPhysique.vit)));
				
	} while (!render.Window()->shouldClose());//ferme la fenetre

	while (Chunk::getNumberOfWorkingThreads());//attends que tous les programmes parallele soit terminé

	return 0;//exit
}


