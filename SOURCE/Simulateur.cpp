#include <SOURCE/Header.h>
#include <nico.hpp>
#include <config.hpp>
#include <TERRAIN/Land.h>
#include "Controls.h"
#include "Deplacement.h"
#include "Forces.h"
#include "interface2d.h"

//simplifie la syntaxe
using namespace nico;
using namespace glm;

// nombre de troncons minimun a chargé avant d'afficher quoi que ce soit
#define NOMBRE_DE_CHUNK_MIN 4
//position par default de la montgolfiere
#define DEFAULT_POS vec3(2554, 8 , 5312)
//temperature par default de la montgolfiere
#define DEFAULT_TEMPERATURE 45


int main() {
	//initialisation de l'afficheur 3D
	Renderer render;
	//mettre une iconne a la fenetre
	render.Window()->setIcon(NICO_TEXTURES_PATH"montgol.png");
	//creer un afficheur de texte avec la police par default
	TextRenderer text(NICO_TEXTURES_PATH"Roboto-Light.ttf");

	//creer la camera
	Camera player;
	render.useCamera(&player);
	player.setZFar(4000);
	player.setPosition(DEFAULT_POS);

	//creer l'afficheur 2D
	Renderer2d render2d(render.Window());

	//creer un switch (on/off) pour le mode pleine ecran, l'affichage supplementaire et le cheat mode
	KeySwitch fullscreen(render.Window(), GLFW_KEY_F11);
	KeySwitch menuInfo(render.Window(), GLFW_KEY_F3);
	KeySwitch godMode(render.Window(), GLFW_KEY_F6);

	//valeurs physiques de la mongolfiere
	float masse(200.0f);
	float volume(1400.0f);
	float temperature = DEFAULT_TEMPERATURE;

	//multiplie le temps
	float temps(1);

	//creation de l'affichage 2D des indicateurs de directions
	Boussole::createTexture();
	Boussole sunBoussole(render.Window(), &player, &text);
	sunBoussole.setName("soleil");
	sunBoussole.setMultiplyColor(vec3(1,1,0));
	render2d.addElement(&sunBoussole);

	Boussole nord(render.Window(), &player, &text);
	nord.setName("N");
	render2d.addElement(&nord);
	Boussole sud(render.Window(), &player, &text);
	sud.setName("S");
	render2d.addElement(&sud);
	Boussole ouest(render.Window(), &player, &text);
	ouest.setName("W");
	render2d.addElement(&ouest);
	Boussole est(render.Window(), &player, &text);
	est.setName("E");
	render2d.addElement(&est);


	nord.setDirection(vec3(1, 0, 0));
	sud.setDirection(vec3(-1, 0, 0));
	ouest.setDirection(vec3(0, 0, -1));
	est.setDirection(vec3(0, 0,  1));

	Boussole vent(render.Window(), &player, &text);
	vent.setName("direction du vent");
	render2d.addElement(&vent);

	//GameOver and pause Interface
	Interface2d interface2d(render.Window(), &text);
	render2d.addElement(&interface2d);
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
	render.Window()->setTitle("Simulateur de montgolfiere v1.0.2-alpha");

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
		temperature = ControleTemperature(render.Window(),temperature, temps);
		interface2d.setThermometer(temperature);
		interface2d.setVariometer(((float)((int)((montgolPhysique.forces.archi.y + montgolPhysique.forces.g.y) * 100)) * 0.01f));
		interface2d.setAltimeter(montgolPhysique.pos.y);

		//creation des forces qui s'appliquent sur la montgolfiere
		montgolPhysique.forces.archi = pousseeDArchimede(masse, volume, temperature, montgolPhysique.pos.y);
		montgolPhysique.forces.vent = ForceDuVent(montgolPhysique.pos.y, glfwGetTime(), temps);
		montgolPhysique.forces.frottements = ForceDeFrottements(montgolPhysique.vit);

		if(godMode)
			montgolPhysique.forces.vent += ControleGodVent(render.Window(), &player);

		//met a jour la boussole du vent
		vent.setDirection(montgolPhysique.forces.vent);

		//fonction qui gere la physique de deplacement
		
		if (!gameOver) {
			//met a jour le deplacement de la mongolfiere et verifie que ce deplacement n'engendre pas une destruction de la motgolfiere
			gameOver = deplacement(&montgolPhysique, render.Window(), terrain.getHitbox(montgolPhysique.pos), temps);

			//si la montoglfiere a été detuite afficher gameover et afficher le curseur
			if (gameOver) {
				interface2d.setState(Interface2d::State::gameOver);
				render.Window()->hideCursor(false);
			}
		}
		else {
			//si l'utilisateur a perdu et qu'il appuit sur reprendre
			if (interface2d.getState() == Interface2d::State::recommencerAuMemeEndroit) {
				gameOver = false;
				montgolPhysique.pos.y = 50;
				montgolPhysique.vit = vec3(0);
				temps = 1;
				interface2d.setState(Interface2d::State::playing);
			}
		}
		//si l'utilisateur appuit sur recommencer
		if (interface2d.getState() == Interface2d::State::recommencer) {
			gameOver = false;
			montgolPhysique.pos = DEFAULT_POS;
			montgolPhysique.vit = vec3(0);
			temperature = DEFAULT_TEMPERATURE;
			temps = 1;
			interface2d.setState(Interface2d::State::playing);

			//il est possible que la translation que l'on viens d'effectuer amene le ballon sur des trocons non chargée 
			//on verifie donc qu'il y a un nombre suffisant de chunks chargé avant d'afficher (comme au début du programme)
			do {
				terrain.update();
			} while (terrain.getNumberOfLoadedChunks() < NOMBRE_DE_CHUNK_MIN);
				
		}
		//si l'utilisateur appuit sur echap
		else if (interface2d.getState() == Interface2d::State::pause) {
			temps = 0;
		}
		//si l'utilisateur a mis pause et qu'il appuit sur reprendre
		else if (interface2d.getState() == Interface2d::State::reprendre) {
			temps = 1;
			interface2d.setState(Interface2d::State::playing);
			render.Window()->setCursorPos(render.Window()->getWidth() * .5, render.Window()->getHeight() * .5);
		}
		//si l'utilisateur est entrain de jouer
		if (interface2d.getState() == Interface2d::State::playing) {
			temps = interface2d.getTimeAcceleration();
		}

			
			
		//afficher la montgolfiere a la nouvelle position 
		montgol.setPos(montgolPhysique.pos);
		soupape.setPos(montgolPhysique.pos + animationSoupape(render.Window()));
		animationBruleur(render, bruleur, montgolPhysique.pos);

		//met a jour la camera avec la souris
		if (interface2d.getState() == Interface2d::State::playing)
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

		//affichage supplementaire (activation avec F3)
		if (menuInfo) {
			text.printLeftTop("loc : " + nico::strings::ivec3Tostring(montgolPhysique.pos) +
				" chunks : " + std::to_string(terrain.getNumberOfLoadedChunks()));
		
			std::string t = "fps : " + std::to_string(render.Window()->getFps());
			glm::vec2 size = text.calculateTextRenderingSize(t, 20.0f);
			text.printRightTop(t,  size.x + 5, size.y + 5, 20.0f);
		}
		
				
	} while (!render.Window()->shouldClose());//ferme la fenetre

	//attendre une seconde que les thread paralelle soit terminé
	for (size_t i = 0; i < 1000 and Chunk::getNumberOfWorkingThreads(); i++)
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	
	return 0;//exit
}

//pour creer une application windows (la configuration release n'a pas de cmd, la fonction main est donc remplacée par WinMain)
#ifdef NDEBUG
int WinMain(){
	return main(); 
}
#endif // NDEBUG