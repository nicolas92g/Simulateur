/*****************************************************************//**
 * \file   nico.hpp
 * \brief  bunch of classes to render 3D model in the format .obj and some 2D stuff like buttons and textInput (with open-gl only)
 * @version 0
 * \author Nicolas GUILLOT
 * \date   November 2020 -> march 2021 (for now)
 *********************************************************************/

#include "nico/window.h"
#include "nico/Camera.h"
#include "nico/glDebugging.h"
#include "nico/imageObject.h"
#include "nico/nicoFunctions.h"
#include "nico/openGlObject.h"
#include "nico/Shader.h"
#include "nico/textRenderer.h"
#include "nico/Object3d.h"
#include "nico/Texture.h"
#include "nico/vertexBuffer.h"
#include "nico/Vertex.h"
#include "nico/mesh.h"
#include "nico/light.h"
#include "nico/Model.h"
#include "nico/cube.h"
#include "nico/Line.h"
#include "nico/FrameBuffer.h"
#include "nico/renderer.h"
#include "nico/Classes.h"
#include "nico/CubeMap.h"
#include "nico/Renderer2d.h"
#include "nico/Editor.h"
#include "nico/Configurable.h"
#include "nico/Object2d.h"
#include "nico/Button.h"
#include "nico/TextInput.h"
#include "nico/NumberInput.h"
#include "nico/Billboard.h"
#include "nico/InstancedModel.h"
