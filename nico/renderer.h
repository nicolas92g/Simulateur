#pragma once
#include <list>
#include "Shader.h"
#include "Object3d.h"
#include "light.h"
#include <nico/Classes.h>
#include <nico/FrameBuffer.h>
#include <nico/window.h>
#include <nico/Camera.h>
#include "CubeMap.h"


//TEXTURE SAMPLER (materials Textures are set automatically(from 0 to the number of materials type of texture available) )

#define NICO_IRRADIANCE_MAP_SAMPLER					28
#define NICO_PREFILTERED_ENVIRONMENT_MAP_SAMPLER	29
#define NICO_BRDF_LUT_MAP							30
#define NICO_SHADOW_MAP_SAMPLER						31

namespace nico {
	/**
	 * @class Renderer
	 * @brief create a window and render all the objects that you add to it
	 */
	class Renderer
	{
	public:
		/**
		 * allow to specify to the renderer which shader it will use to render
		 */
		enum class renderingAlg {
			phong,	// this alg do not need a lot of performance but the result is not amazing
			physicaly_based_rendering // use more performance but is a lot better
		};

	public:
		/**
		 * create the open-gl context and a window
		 */
		Renderer();
		~Renderer();
		/**
		 * call all the entities updates() call
		 */
		void update();
		/**
		 * update frame and render to it in the window
		 */
		void frame(uint32_t frameBuffer = NULL, uint32_t viewportX = NULL, uint32_t viewportY = NULL);
		/**
		 * to call at the start of each frame 
		 * at the beginning of the main loop
		 */
		void clear();
		/**
		 * add an movable element to the the rendering and check in the array
		 * if it is not already in
		 * \param object
		 */
		void addEntity(Object3d* object);
		/**
		 * add a static element to the rendering and check in the array
		 * if it is not already in 
		 * \param object
		 */
		void addDecor(Object3d* object);
		/**
		 * remove an Object considered as an entity of the rendering 
		 * this don't delete the object
		 * \param object
		 */
		void removeEntity(Object3d* object);
		/**
		 * remove an Object considered as an static element of the rendering
		 * this don't delete the object
		 * \param object
		 */
		void removeDecor(Object3d* object);
		/**
		 * add a light source in the rendering, 
		 * you can add all instances of a type witch have nico::BasicLight as a mother
		 * \param lightSource
		 */
		void addLightSource(BasicLight* pointLightSource);
		/**
		 * remove a light of the rendering this don't delete the object
		 * \param lightSource
		 */
		void removeLightSource(BasicLight* pointLightSource);
		/**
		 * set the directional light of the rendering
		 * this renderer allow only one directionalLight
		 * \param sun
		 */
		void setDirectionalLight(DirectionalLight* sun);
		/**
		 * this camera pointer can be modified and readed by the renderer but not deleted
		 * you have to create a camera by yourself
		 * \param camToUseToRender
		 */
		void useCamera(Camera *camToUseToRender);
		/**
		 * allow to use a different algorithm to render
		 * \param algName
		 */
		void setRenderingAlgorithm(renderingAlg algName);
		/**
		 * load an HDR equiRectangular 360d Map and put it in a cubeMap 
		 * res is the resolution of both edges of each squares of the cubeMap 
		 * \param path
		 * \param res
		 */
		void loadEnvironmentMap(const char* path, int res = 2048);
		/**
		 * @brief draw the environment map as a sky map
		 */
		void drawEnvironmentMapAsSkyMap();
		/**
		 * this rotate the skyMap
		 * \param angle
		 * \param vec3
		 */
		void setSkyMapRotation(float angle, glm::vec3 rotAxe = glm::vec3(0,1,0));
		/**
		 * @brief set the light power of the environment map
		 * \param ambientStrength
		 */
		void setAmbientStrength(float ambientStrength);
		/**
		 * @brief set the shaders constant uniforms
		 */
		void setShaderConstantsUniforms(Shader* shader);
		/**
		 * @brief bind all the textures needed, and set some variable uniform
		 */
		void prepareShaderBeforeDraw(Shader* shader);
		/**
		 * @brief send all the lights to the shader
		 * \param shader
		 */
		void sendLightsToShader(Shader* shader);

		//accessors
		/**
		 * return a pointer to the current shader of the renderer
		 * \return 
		 */
		Shader* Shader();
		/**
		 * return a pointer to the window currently opened by the renderer
		 * \return 
		 */
		Window* Window();
			


	protected:
		//WINDOW
		nico::Window win;

		//SHADERS
		nico::Shader* shader;//this is a pointer to the main shader that render all the objects

		nico::Shader shadowShader;//this is use to render shadow depth map
		nico::Shader skyMapShader;//this is use to render the skymap
		nico::Shader equiRectToCubeMap;// this shader is use to put an equiRectangular map into a cubeMap
		nico::Shader irradianceShader;//this is use to create the irradiance map
		nico::Shader prefilterShader;//this generate a pre-filtered environment map with different mip-maps for roughness
		nico::Shader brdfShader;
		
		nico::Shader phongShader;//this is use to render with phong technique
		nico::Shader pbrShader;	 //this is use to render with phisically based rendering techniques
		
		
		
		//CAMERA
		Camera* cam;//camera to use to render

#		ifdef NICO_RENDERING_DEBUG
		Key reloadShaders;//reload the shaders key
#		endif

		//DYNAMIC SHADOWS OF THE SUN
		FrameBuffer shadowDepthFBO;//framebuffer for the shadows
		uint32_t shadowDepthTextureId;//open gl id of the texture used as a depth map
		uint32_t shadowTextureSize;//the texture is a square so this number is the length of each edges
		glm::mat4 lightVP;//this is the multiplication of the orthographic projection and the view matrices

		//configure the framebuffer and his texture
		void initShadowSystem(uint32_t depthMapSize);
		//draw all the objects in the depth map
		void updateShadows(vec3 position, vec3 lightDirection, float sizeOfView);


		//DRAWABLE STUFFS
		std::list<Object3d*> entities;
		std::list<Object3d*> decors;

		//PBR RENDERING STUFF
		uint32_t environmentMap;// just a image of the environment
		uint32_t irradianceMap;	// irradiance of each direction of the environment
		uint32_t preFilteredEnvironmentMap;//store a environment map with multiple mip-maps for differents roughness
		uint32_t brdfMap;

		CubeMesh cube;//this is needed to render and create cubeMaps
		uint32_t planeVAO;//use to render flat textures
		
		void updateIrradianceMap();	//this create a irradiance map from the environment map
		void updatePreFilteredEnvironmentMap();// this create the pre-filtered cubemap for specular pbr 
		void updateBRDFpreComputing();

		void createPlaneMesh();//configure plane mesh

		//LIGHTS
		std::list<BasicLight*> pointLights;//this use polymorphysm to render different lights
		DirectionalLight* dirLight;//this is a pointer to the only directional of the scene 

		//DISPLAYS
		void drawObjects();
		
		//array treatement functions
		template<typename T>
		void addElement(T* element, std::list<T*>* list);
		template<typename T>
		void removeElement(T* element, std::list<T*>* list);
		

	public:
	
	};


	//inline declaration :
	
	template<typename T>
	inline void Renderer::addElement(T* element, std::list<T*>* list)
	{
		for (typename std::list<T*>::iterator it = list->begin(); it != list->end(); it++) {
			if ((*it) == element)
				return;
		}
		list->push_back(element);
	}

	template<typename T>
	inline void Renderer::removeElement(T* element, std::list<T*>* list)
	{
		for (typename std::list<T*>::iterator it = list->begin(); it != list->end(); it++) {
			if ((*it) == element) {
				list->erase(it);
				return;
			}
		}
	}
}
