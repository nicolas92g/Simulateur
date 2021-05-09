#include "nico.hpp"
#include "renderer.h"

nico::Renderer::Renderer() :
	//constructors of the vars
#	ifdef NICO_RENDERING_DEBUG
	win(4, 3, true, 4),//start open gl in debug mode
	reloadShaders(&win, GLFW_KEY_F5),
#	else
	win(4, 3, false, 4),//start open gl in release mode
#	endif
	shadowDepthFBO(),//initialize the dynamics shadows framebuffer
	//shaders paths :
	phongShader(NICO_SHADERS_PATH"default.vert", NICO_SHADERS_PATH"default.frag"),
	shadowShader(NICO_SHADERS_PATH"shadow.vert", NICO_SHADERS_PATH"shadow.frag"),
	pbrShader(NICO_SHADERS_PATH"default.vert", NICO_SHADERS_PATH"pbr.frag"),
	skyMapShader(NICO_SHADERS_PATH"skyBox.vert", NICO_SHADERS_PATH"skyBox.frag"),
	equiRectToCubeMap(NICO_SHADERS_PATH"equirectangularToCubemap.vert", NICO_SHADERS_PATH"equirectangularToCubemap.frag"),
	irradianceShader(NICO_SHADERS_PATH"irradianceMap.vert", NICO_SHADERS_PATH"irradianceMap.frag"),
	prefilterShader(NICO_SHADERS_PATH"preFilter.vert", NICO_SHADERS_PATH"preFilter.frag"),
	brdfShader(NICO_SHADERS_PATH"brdf.vert", NICO_SHADERS_PATH"brdf.frag"),
	shader(&pbrShader)//set the shader by default

//body of the renderer constructor :

{
	win.setTitle("loading...");

	shadowDepthFBO.unbind();
	
	//initialisation functions
	initShadowSystem(10000);
	createPlaneMesh();
	setShaderConstantsUniforms(this->shader);
	
	//enable default depth testing
	glEnable(GL_DEPTH_TEST);

	//enable transparency with the alpohe color channel
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//do not display a face when you are not in front of it
	glEnable(GL_CULL_FACE);

	//avoid visible cube edges of the cubemaps 
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	//framerate is as high as possible
	glfwSwapInterval(0);

	//set a sky blue color as a clear color by default
	glClearColor(0.7f, 1, 1.0f, 1);
}

nico::Renderer::~Renderer()
{
}

void nico::Renderer::update()
{
	//update all the objects
	for (std::list<Object3d*>::iterator it = entities.begin(); it!=entities.end(); it++)
	{
		(*it)->update();
	}

	//update dynamic shadow depth map
	updateShadows(shadowCubeCenter, dirLight->getDirection(), shadowCubeSize);
}

void nico::Renderer::frame(uint32_t frameBuffer, uint32_t viewportX, uint32_t viewportY)
{
	//update camera
#	ifdef NICO_RENDERING_DEBUG
	

	//press F5 to reload pbr shader
	if (reloadShaders.isDown()) {
		std::cout << "reloading shaders...\n";
		nico::Shader newShader(NICO_SHADERS_PATH"default.vert", NICO_SHADERS_PATH"pbr.frag");
		pbrShader = newShader;
		shader = &pbrShader;
		setShaderConstantsUniforms(this->shader);
	}

	reloadShaders.getState();

#	endif
	//camera :
	cam->calculateMatrix(&win);
	cam->sendToShader(shader);


	sendLightsToShader(shader);

	//framebuffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	if(!viewportX)
		glViewport(0, 0, win.getWidth(), win.getHeight());
	else
		glViewport(0, 0, viewportX, viewportY);


	//display of the objects
	drawObjects();
}

void nico::Renderer::clear(uint32_t frameBuffer)
{
	win.endFrame();
	nico::clearLights();
	TextRenderer::textToPrint = "";

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void nico::Renderer::addEntity(Object3d* object)
{
	addElement<Object3d>(object, &entities);
}

void nico::Renderer::addDecor(Object3d* object)
{
	addElement<Object3d>(object, &decors);
}

void nico::Renderer::removeEntity(Object3d* object)
{
	removeElement<Object3d>(object, &entities);
}

void nico::Renderer::removeDecor(Object3d* object)
{
	removeElement<Object3d>(object, &decors);
}

void nico::Renderer::addLightSource(BasicLight* lightSource)
{
	addElement(lightSource, &pointLights);
}

void nico::Renderer::removeLightSource(BasicLight* lightSource)
{
	removeElement(lightSource, &pointLights);
}

void nico::Renderer::setDirectionalLight(DirectionalLight* sun)
{
	dirLight = sun;
}

void nico::Renderer::useCamera(nico::Camera* camToUseToRender)
{
	this->cam = camToUseToRender;
}

void nico::Renderer::setRenderingAlgorithm(renderingAlg algName)
{
	switch (algName)
	{
	case nico::Renderer::renderingAlg::phong:
		shader = &phongShader;
		break;
	case nico::Renderer::renderingAlg::physicaly_based_rendering:
		shader = &pbrShader;
		break;
	default:
		break;
	}
	setShaderConstantsUniforms(this->shader);
}

void nico::Renderer::setSkyMapRotation(float angle, glm::vec3 axe)
{
	const glm::mat4 rot = glm::rotate(angle, axe);

	skyMapShader.set("rotation", rot);
	shader->set("environmentRotation", rot);
}

void nico::Renderer::setAmbientStrength(float value)
{
	shader->set("ambientStrength", value);
}

nico::Shader* nico::Renderer::Shader()
{
	return shader;
}

nico::Window* nico::Renderer::Window()
{
	return &win;
}

void nico::Renderer::setShaderConstantsUniforms(nico::Shader* shader)
{
	//renderer shader
	shader->set("irradianceMap",	NICO_IRRADIANCE_MAP_SAMPLER);
	shader->set("prefilterMap",		NICO_PREFILTERED_ENVIRONMENT_MAP_SAMPLER);
	shader->set("brdfLUT",			NICO_BRDF_LUT_MAP);
	shader->set("shadowDepthMap",	NICO_SHADOW_MAP_SAMPLER);

	shader->set("environmentRotation", glm::rotate(glm::radians(.0f), glm::vec3(0,1,0)));
	shader->set("ambientStrength", 1.0f);

	//skymap shader
	skyMapShader.set("skybox", NULL);
	skyMapShader.set("rotation", glm::rotate(glm::radians(0.0f), vec3(0, 1, 0)));

	//make the HDR skymap shader 
	equiRectToCubeMap.set("equirectangularMap", NULL);

	//make the irradiance map shader
	irradianceShader.set("environmentMap", NULL);

	//make the perfiltered environment map shader
	prefilterShader.set("environmentMap", NULL);
}

void nico::Renderer::prepareShaderBeforeDraw(nico::Shader* shader)
{
	//dynamics shadows shader updates
	shader->set<glm::mat4>("lightSpaceMatrix", lightVP);

	//dynamics shadows texture binding
	glActiveTexture(GL_TEXTURE0 + NICO_SHADOW_MAP_SAMPLER);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTextureId);

	//binding irradiance map
	glActiveTexture(GL_TEXTURE0 + NICO_IRRADIANCE_MAP_SAMPLER);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	//bind pre-filtered environment map
	glActiveTexture(GL_TEXTURE0 + NICO_PREFILTERED_ENVIRONMENT_MAP_SAMPLER);
	glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredEnvironmentMap);

	//bind pre-filtered environment map
	glActiveTexture(GL_TEXTURE0 + NICO_BRDF_LUT_MAP);
	glBindTexture(GL_TEXTURE_2D, brdfMap);
}

void nico::Renderer::sendLightsToShader(nico::Shader* shader)
{
	//add lights to rendering 
	for (std::list<nico::BasicLight*>::iterator it = pointLights.begin(); it != pointLights.end(); it++) {
		(**it).sendToShader(shader);
	}

	//set the directional light in the shader
	if (dirLight != nullptr)
		dirLight->sendToShader(shader);
}

void nico::Renderer::setShadows(const glm::vec3& location, float size)
{
	shadowCubeCenter = location;
	if (size > .1f) shadowCubeSize = size * .5f;
}

void nico::Renderer::initShadowSystem(uint32_t depthMapSize)
{
	//this stock the size of the generated texture
	this->shadowTextureSize = depthMapSize;

	//configure the texture that will be fill by the framebuffer
	glGenTextures(1, &shadowDepthTextureId);
	glBindTexture(GL_TEXTURE_2D, shadowDepthTextureId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, depthMapSize, depthMapSize, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	//depth texture parameters:
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	
	//set the outer depth texture color
	const float borderColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	//configure the depth frame buffer and attach the texture to it 
	shadowDepthFBO.bind();
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTextureId, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	shadowDepthFBO.unbind();

}

void nico::Renderer::updateShadows(vec3 position, vec3 lightDirection, float sizeOfView)
{

	shadowDepthFBO.bind();
	glViewport(0, 0, shadowTextureSize, shadowTextureSize);
	glClear(GL_DEPTH_BUFFER_BIT);

	//glCullFace(GL_FRONT);
	const bool faceCulling = glIsEnabled(GL_CULL_FACE);
	if (faceCulling)
		glDisable(GL_CULL_FACE);
	
	glCullFace(GL_FRONT);

	glm::mat4 lightProjection = glm::ortho<float>(-sizeOfView, sizeOfView, -sizeOfView, sizeOfView, -sizeOfView, sizeOfView);
	glm::mat4 lightView = glm::lookAt<float>(-lightDirection + position, position, vec3(0, 1, 0));

	lightVP = lightProjection * lightView;

	shadowShader.set<glm::mat4>("lightSpaceMatrix", lightVP);

	//for (std::list<Object3d*>::iterator it = decors.begin(); it != decors.end(); it++) {
	//	(**it).draw(&shadowShader);
	//}
	for (std::list<Object3d*>::iterator it = entities.begin(); it != entities.end(); it++) {
		(**it).draw(&shadowShader);
	}
	glCullFace(GL_BACK);

	if (faceCulling)
		glEnable(GL_CULL_FACE);
}

void nico::Renderer::loadEnvironmentMap(const char* path, int res)
{
	stbi_set_flip_vertically_on_load(true);
	int width, height, nrComponents;
	float* data = stbi_loadf(path, &width, &height, &nrComponents, 0);
	unsigned int hdrTexture = 0;
	if (data)
	{
		glGenTextures(1, &hdrTexture);
		glBindTexture(GL_TEXTURE_2D, hdrTexture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "CubeMap error : can't load file : " << path << std::endl;
	}

	unsigned int captureFBO, captureRBO;
	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, res, res);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, captureRBO);

	glGenTextures(1, &environmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		// note that we store each face with 16 bit floating point values
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F,
			res, res, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// convert HDR equirectangular environment map to cubemap equivalent
	equiRectToCubeMap.set("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, res, res); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		equiRectToCubeMap.set<glm::mat4>("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, environmentMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bool cull = glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);

		cube.draw(&equiRectToCubeMap);

		if (cull)
			glEnable(GL_CULL_FACE);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	updateIrradianceMap();
	updatePreFilteredEnvironmentMap();
	updateBRDFpreComputing();
}

void nico::Renderer::drawEnvironmentMapAsSkyMap()
{
	glDepthFunc(GL_LEQUAL);

	glDepthMask(GL_FALSE);

	skyMapShader.set("hdr", true);
	skyMapShader.set("view", glm::mat4(glm::mat3(cam->getView())));
	skyMapShader.set("projection", cam->getProjection());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	bool cull = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	cube.draw(&skyMapShader);

	if (cull)
		glEnable(GL_CULL_FACE);

	glDepthMask(GL_TRUE);
}

void nico::Renderer::updateIrradianceMap()
{

	unsigned int captureFBO = NULL;
	unsigned int captureRBO = NULL;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);

	glGenTextures(1, &irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);

	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 32, 32, 0,
			GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	
	irradianceShader.set("projection", captureProjection);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	for (unsigned int i = 0; i < 6; ++i)
	{
		irradianceShader.set("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
			GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, irradianceMap, 0);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		bool cull = glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);

		cube.draw(&irradianceShader);

		if (cull)
			glEnable(GL_CULL_FACE);

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void nico::Renderer::updatePreFilteredEnvironmentMap()
{
	unsigned int captureFBO = NULL;
	unsigned int captureRBO = NULL;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);

	glGenTextures(1, &preFilteredEnvironmentMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, preFilteredEnvironmentMap);
	for (unsigned int i = 0; i < 6; ++i)
	{
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	glm::mat4 captureViews[] =
	{
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
	   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	prefilterShader.set("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, environmentMap);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	unsigned int maxMipLevels = 5;
	for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
	{
		// resize framebuffer according to mip-level size
		unsigned int mipWidth = 128.0f * std::pow(.5f, mip);
		unsigned int mipHeight = 128.0f * std::pow(.5f, mip);
		glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		float roughness = (float)mip / (float)(maxMipLevels - 1);
		prefilterShader.set("roughness", roughness);
		for (unsigned int i = 0; i < 6; ++i)
		{
			prefilterShader.set("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, preFilteredEnvironmentMap, mip);

			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			bool cull = glIsEnabled(GL_CULL_FACE);
			glDisable(GL_CULL_FACE);

			cube.draw(&prefilterShader);

			if (cull)
				glEnable(GL_CULL_FACE);
		}
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void nico::Renderer::updateBRDFpreComputing()
{

	bool blend = glIsEnabled(GL_BLEND);
	glDisable(GL_BLEND);
	
	unsigned int captureFBO = NULL;
	unsigned int captureRBO = NULL;

	glGenFramebuffers(1, &captureFBO);
	glGenRenderbuffers(1, &captureRBO);
	glGenTextures(1, &brdfMap);

	// pre-allocate enough memory for the LUT texture.
	glBindTexture(GL_TEXTURE_2D, brdfMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, 512, 512, 0, GL_RG, GL_FLOAT, 0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, captureRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 512, 512);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, brdfMap, 0);

	glViewport(0, 0, 512, 512);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	bool cull = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	
	brdfShader.use();
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);	

	if (cull)
		glEnable(GL_CULL_FACE);

	if (blend)
		glEnable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void nico::Renderer::createPlaneMesh()
{
	std::vector<float> vert = {
		// positions			// texture Coords
		-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
		 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
	};

	glGenVertexArrays(1, &planeVAO);
	glBindVertexArray(planeVAO);

	VertexBuffer<float> vbo(&vert, 5);

	vbo.addLayout(NICO_POSITIONS);
	vbo.addLayout(NICO_TEXTURESUV);

	glBindVertexArray(0);
}

void nico::Renderer::drawObjects()
{

	//TEST A 2D TEXTURE HERE
	/*shader->use();
	shader->set("model", glm::translate(vec3(0, 5, 0)));
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, brdfMap);
	shader->set("diffuse", 3);
	
	glBindVertexArray(planeVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);*/


	//DRAWING CALLS :

#	ifdef NICO_RENDERING_DEBUG
	int nbr = 0;
#	endif

	glDepthFunc(GL_LEQUAL);

	prepareShaderBeforeDraw(this->shader);

	//iterate through decor
	for (std::list<Object3d*>::iterator it = decors.begin(); it != decors.end(); it++) {
		//draw call:
		if (glm::distance((**it).getPos(), cam->getPosition()) < 1000 and
			(cam->isVertexInTheFieldOfView((**it).getPos()) or glm::distance((**it).getPos(), cam->getPosition()) < 10)) {
			(**it).draw(shader);

			//number of drawing objects counting
#		ifdef NICO_RENDERING_DEBUG
			nbr++;
#		endif
		}
	}

	//iterate through entities
	for (std::list<Object3d*>::iterator it = entities.begin(); it != entities.end(); it++) {
		//draw call: 
		(**it).draw(shader);

		//number of drawing objects counting
#		ifdef NICO_RENDERING_DEBUG
		nbr++;
#		endif
	}

#	ifdef NICO_RENDERING_DEBUG
	TextRenderer::textToPrint += " objs : " + std::to_string(nbr);
#	endif
}
