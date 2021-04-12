#include <SOURCE/Header.h>
#include <nico.hpp>
#include <config.hpp>
#include <TERRAIN/Land.h>
#include "Controls.h"
#include "Deplacement.h"

using namespace nico;
using namespace glm;


int main() {

	Renderer render;
	Camera player;
	render.useCamera(&player);
	player.setZFar(4000);

	Model montgolGeo(NICO_PATH"MODELISATION/baloon.obj");
	Object3d montgol(&montgolGeo);
	render.addEntity(&montgol);

	Physique montgolPhysique;
	montgolPhysique.forces.g = vec3(0, -9.81, 0);
	montgolPhysique.vit = vec3(0, 50, 40);
	montgolPhysique.pos = vec3(-458, 3.6, 3470);
	
	Controls souris(&player, &montgol, render.Window(),vec3(0, 2, 0));
	souris.minAndMaxAngles =(vec2(-0.99, 0));

	DirectionalLight sun;
	sun.setColor(vec3(3));
	render.setDirectionalLight(&sun);

	render.loadEnvironmentMap(NICO_TEXTURES_PATH"sky.hdr");

	Land terrain(&render, &player);
	render.Window()->maximise();

	do {
		render.clear();


		deplacement(&montgolPhysique, render.Window());
		montgol.setPos(montgolPhysique.pos);

		souris.update();

		render.update();
		render.frame();
		render.drawEnvironmentMapAsSkyMap();

		terrain.update();
		terrain.draw();
	} while (!render.Window()->shouldClose());
	return 0;
}


