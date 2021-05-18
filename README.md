# Simulateur
Simulateur de Mongolfiere (hot air balloon Simulator)

press escape and then on the ? button on the application to know how to use it.

description of the project:
This Simulator was written in C++ with openGL
additional used libraries are : glfw, glm, freetype, libnoise, stb-master, glad, assimp

it use a terrain proceduraly generated with a noise function.

it is able to simulate the Archimedes thrust by taking some constant volume and mass but also by determining the presure of the outer air in function of the altitude. The mass of the internal air is calculated with the temperature of the balloon and there is a basic friction simulation to avoid huge speed. vector g is of course (0, -9.81, 0) and there is some randoms winds that changed randomly with the altitude.

constrols used : 
escape to open menu
left control to open valve
space to heat balloon
mouse to choose a camera angle
scroll wheel to zoom 
F11 to enable fullscreen
left and right arrows to warp time
F6 to enable WASD wind control(cheat code !)
F3 to display framerate
