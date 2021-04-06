#include <nico.hpp>// cette ligne inclut toutes les biblioteques dont on a besoin et mes classes aussi 
#include <TERRAIN/Land.h>

using namespace nico;//on est pas obligé de taper "nico::" devant tous mes classes (nico::Renderer par exemple c chiant)
using namespace glm;//ca c'est la biblioteque de fonctions mathématiques et cette ligne evite qu'on ai a mettre "glm::" devant les vecteurs par example


//programme de départ
int programmeMinimun() {
	Renderer render;
	Camera player;
	render.useCamera(&player);

	DirectionalLight sun;
	render.setDirectionalLight(&sun);

	render.loadEnvironmentMap(NICO_TEXTURES_PATH"Ridgecrest_Road_Ref.hdr");

	do {
		render.clear();
		render.update();
		render.frame();
	} while (!render.Window()->shouldClose());
	return 0;
}

//tous ce qui necessaire au fonctionement du programme est commenté par : // ! //

int exempleDeProgrammeComplet()
{
	//initialisation du monde 3D 
	//---------------------------------------
	Renderer render;// ! //on créé l'afficheur 3D
	Camera player(vec3(3, 6, -2));// ! // on créé la camera qui "filme" le monde avec une position initiale de (x = 3, y = 6, z = -2)
	player.setZFar(4000);
	render.useCamera(&player);// ! //on dit a notre afficheur qu'il doit afficher dans cette camera

	//creation du cube
	Model cubeModel("C:/Users/nicol/OneDrive/Documents/Graphismes/models/metal/metal.obj");//on charge une geometrie 3D depouis un fichier .obj créé avec blender
	Object3d cube(&cubeModel, vec3(2, 2, -1));//on créé un object 3d qui a une position, une taille, une rotation et qui a une geometrie 3D
	render.addEntity(&cube);// on met cet objet 3D dans notre afficheur

	//soleil
	DirectionalLight sun(vec3(-0.5, -0.2, -0.4));// ! // on cree une lumière infiniment loin et infiniment puissante avec une direction initiale
	render.setDirectionalLight(&sun);// ! // on met cette lumiere a notre afficheur
	sun.setColor(vec3(3));

	//lumiere
	Light point;//on créé une lumière
	point.setPos(cube.getPos() + vec3(3, 0.5, 1));//on modifie sa position
	point.setColor(vec3(50, 10, 0.5f));//on modifie sa couleur
	point.setLightAttenuationValues(0.6f);//on modifie son attenuation en fonction de la distance faut mettre un valeur comprise dans ]0, 1[ 
	render.addLightSource(&point);//ajoute cette lumiere dans notre afficheur

	//spot de lumiere
	SpotLight spot;//on créé un spot
	spot.setPos(cube.getPos() + vec3(-10, -1.5, 6));//on modifie sa position
	spot.setSpotAngle(20, 25);
	spot.setDirection(vec3(1, -0.03, 0.1));//on modifie la direction du spot
	spot.setColor(vec3(29, 22, 40.5f));//on modifie sa couleur
	spot.setLightAttenuationValues(0.05f);//on modifie son attenuation en fonction de la distance faut mettre un valeur comprise dans ]0, 1[ 
	render.addLightSource(&spot);//ajoute cette lumiere dans notre afficheur



	// ! //ca c'est pour charger une texture d'environment de format .hdr (vous pouvez en trouver sur internet en cherchant ca : hdr environment map )
	render.loadEnvironmentMap(NICO_TEXTURES_PATH"skyMap.hdr", 4000);// ! //kloppenheim_06_16k
	//----------------------------------------
	//initialisation du monde 2d
	Renderer2d render2d(render.Window());// on cree l'afficheur 2D en lui donnant juste la fenetre dans laquel il doit afficher

	Texture haloTexture(NICO_TEXTURES_PATH"halo.png");//on charge une image .png ou .jpg ou quelques autre format marche
	Object2d halo(&haloTexture, vec2(10), vec2(10));//on cree un object 2D qui contient une image, une position, une taille, une rotation
	render2d.addElement(&halo);//on ajoute cet objet 2d a notre afficheur 2D
	halo.setMultiplyColor(vec3(1, 0.1, 1));// ajoute un filtre de couleur sur la texture de base de l'objet 

	NumberInput input(render.Window());//on créé une entrée de nombre réél qui fonctionne comme un object2d
	render2d.addElement(&input);//on ajoute cette objet 2D a notre afficheur
	input.setPosition(vec2(render.Window()->getWidth() / 4, render.Window()->getHeight() / 4));//on change la position de l'entrée nombre
	input.setTransparency(0.5f);//on met la transparence a la moitié --> ]0, 1]
	input.setValue(10);//mettre une valeur par default a notre entrée
	//--------------------------------------------

	// quelques fonctions utiles :

	//changer le titre de la fenetre
	render.Window()->setTitle("COUCOU");

	//changer la puissance de la reflection de la texture d'environment sur les objets [0, 1]
	render.setAmbientStrength(0.3f);

	//ca c'est l'objet du terrain il prends comme parametre un afficheur 3D, et une camera
	Land land(&render, &player);

	do {// ! // A PARTIR DE CETTE LIGNE, CHAQUE LIGNE S'EXECUTE A CHAQUE IMAGE DONC RACONTER VOTRE VIE SVP SI VOUS VOULEZ DES FPS

		render.clear();// ! //nettoie l'ecran pour permetre d'y afficher une nouvelle image

		//change dynamiquement la rotation de l'objet 3D "sphere"
		cube.setRotation(vec3(1, sin(render.Window()->getTime() * 0.2f) * 4, 1), render.Window()->getTime() * 0.01f * input.getValue());

		//on change dynamiquement la position l'object 2D "halo" au millieu de l'ecran
		halo.setPosition(vec2(render.Window()->getWidth() / 2, render.Window()->getHeight() / 2));

		float speed = 5;
		if (render.Window()->Key(GLFW_KEY_LEFT_SHIFT))
			speed = 50;

		player.classicKeyboardControls(render.Window(), speed);// ! //permet de bouger dans le monde avec z q s d espace et control - gauche
		player.classicMouseControls(render.Window(), 0.004f);// ! //permet de bouger la direction de la camera avec la souris

		render.update();// ! //met a jour l'affichage des object 3D
		render.frame();// ! //affiche une image a l'ecran des objets 3D
		
		render.drawEnvironmentMapAsSkyMap();

		
		land.update();//met a jour le terrain
		land.draw();//affiche le terrain

		
		if (player.getPosition().y > 100)
			player.setZNear(2);//modifie la distance mimimale que peut percevoir la camera
		else
			player.setZNear(0.2);


		//2d
		render2d.frame();// affiche tous les object 2D

		//on teste si la touche echap est appuyé et si c'est le cas on ferme la fenetre
		//liste des noms des touches sur : https://www.glfw.org/docs/latest/group__keys.html
		if (render.Window()->Key(GLFW_KEY_ESCAPE)) {
			render.Window()->setShouldClose(true);//on demande a fermer la fenetre
		}

	} while (!render.Window()->shouldClose());// ! //la boucle continue tant la fenetre ne demande PAS a etre fermer

	while (Chunk::getNumberOfWorkingThreads());//attends que tous les programmes parallele soit terminé

	return 0;// la simulation est finie (enfin)
}

int main() {
	return exempleDeProgrammeComplet();
}
