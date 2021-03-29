/*****************************************************************//**
 * \file   config.hpp
 * \brief  this is a file that configure the lib
 * \author nicol
 * \date   January 2021
 *********************************************************************/


//this is the path of nico directory
#define NICO_PATH "C:/Users/nicol/OneDrive/Documents/C++ libs/nico/"//LA faut le repertoire dans lequel vous avez mis le dossier nico

//this enable some rendering debugging tools but decrease performances
#define NICO_RENDERING_DEBUG



//apres on s'en fout

//ca ca marche pas(voila voila)
#ifdef NICO_RENDERING_DEBUG
#define GHOST_VIEW_MODE_KEY GLFW_KEY_F4
#endif

//shader layouts 
#define NICO_POSITIONS	{0, 3}
#define NICO_NORMALS	{1, 3}
#define NICO_TEXTURESUV {2, 2}
#define NICO_TANGENTS	{3, 3}
#define NICO_BITANGENTS	{4, 3}

//paths of the shaders and textures
#define NICO_SHADERS_PATH NICO_PATH"nico/shaders/"
#define NICO_TEXTURES_PATH NICO_PATH"nico/objects/textures/"