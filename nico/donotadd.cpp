#include <nico.hpp>
//#include <json/json.h>

#define KEY(key) render.Window()->Key(key)


using namespace nico;
using namespace glm;

int openGL() {
	Renderer render;//3D rendering
	Renderer2d render2d(render.Window());
	TextRenderer text;//2D text renderer

	render.Window()->setIcon(NICO_TEXTURES_PATH"montgol.png");

	Camera player;
	player.setZFar(200.0f);
	player.setPosition(vec3(0, 2, 0));
	render.useCamera(&player);

	Light light1;
	//test
	Shader billboardShader(NICO_SHADERS_PATH"billboard.vert", NICO_SHADERS_PATH"billboard.frag", NICO_SHADERS_PATH"billboard.geom");
	
	Texture imageBill(NICO_TEXTURES_PATH"button.png");
	Billboard bill(&imageBill, &player);
	std::vector<float> billPos = { 0, 2, 0 };
	//bill.setLocations(&billPos);
	
	SpotLight handSpot;
	Light torch0;

	handSpot.setColor(vec3(50));
	handSpot.setLightAttenuationValues(0.2f);
	KeySwitch handSpotOn(render.Window(), GLFW_KEY_Q, true);

	torch0.setPos(vec3(5, 4, -3));
	torch0.setColor(vec3(.9f, 0, 0));
	torch0.setRange(Light::range::_100);
	

	DirectionalLight sunLight;
	sunLight.setColor(vec3(10));
	sunLight.setDirection(vec3(0.2f, -1.0f, 0.35f));

	ImageMesh groundMesh("wood.jpg", 20);
	CubeMesh cubeMesh;
	//Plane plane;

	Model brick("C:/Users/nicol/OneDrive/Documents/Graphismes/models/brick/brickFloor.obj");
	//Model backpackModel("C:/Users/nicol/OneDrive/Documents/C++ libs/nico/nico/objects/backpack/backpack.obj");
	Model souche("C:/Users/nicol/OneDrive/Documents/Graphismes/models/souche/souche.obj");
	Model metalPbr("C:/Users/nicol/OneDrive/Documents/Graphismes/models/metal/metal.obj");
	Model BallonModel("C:/Users/nicol/OneDrive/Documents/Graphismes/models/starship/starship.obj");
	Model pbrDemo("C:/Users/nicol/OneDrive/Documents/Graphismes/models/pbrDemo/pbrDemo.obj");
	Model rocky("C:/Users/nicol/OneDrive/Documents/Graphismes/models/rockyFloor/rockyFloor.obj");

	Object3d backpack(&pbrDemo, vec3(2, 1.7f, 6), vec3(1), vec3(0,1,0), glm::radians(-90.0f)); 
	Object3d tree(&souche, vec3(3, -0.001, -4), vec3(1));
	Object3d cube(&cubeMesh, vec3(-5, 1, -6), vec3(.6f));
	Object3d wall(&brick, vec3(-3,2,5), vec3(2), vec3(1, 0, 0), glm::radians(-90.0f));
	Object3d ground(&rocky, vec3(0), vec3(1), vec3(1, 0, 0), glm::radians(-00.0f));
	Object3d metal(&metalPbr, vec3(0,1,0));
	Object3d balloon(&BallonModel, vec3(2, 8, 1));

	//Light light1; light1.setName("light1"); light1.importFromJson(R"({"light1": {"color": {"r": 50,"g" : 50,"b" : 40}}})");
	Light light2;
	Light light3;
	Light light4;

	light1.setColor(vec3(10)); light1.setPos(vec3( 4, 2, -4));		light1.setLightAttenuationValues(0.5f);
	light2.setColor(vec3(10)); light2.setPos(vec3(-4, 2, -4));		light2.setLightAttenuationValues(0.5f);
	light3.setColor(vec3(10)); light3.setPos(vec3(-4, 0.4f, 4));	light3.setLightAttenuationValues(0.5f);
	light4.setColor(vec3(10)); light4.setPos(vec3(4, 2, 4));		light4.setLightAttenuationValues(0.5f);
	
	

	Object3d light1obj(&metalPbr, light1.getPos(), vec3(0.1f));
	Object3d light2obj(&metalPbr, light2.getPos(), vec3(0.1f));
	Object3d light3obj(&metalPbr, light3.getPos(), vec3(0.1f));
	Object3d light4obj(&metalPbr, light4.getPos(), vec3(0.1f));

	NumberInput light1Strength(render.Window());
	light1Strength.setTransparency(0.4f);
	light1Strength.setStep(0.5f);
	render2d.addElement(&light1Strength);

	Button openMenu(render.Window());
	render2d.addElement(&openMenu);

	

	render.addEntity(&light1obj);
	//render.addEntity(&light2obj);
	//render.addEntity(&light3obj);
	//render.addEntity(&light4obj);

	render.addLightSource(&light1);
	//render.addLightSource(&light2);
	//render.addLightSource(&light3);
	//render.addLightSource(&light4);

	render.addEntity(&backpack);
	//render.addEntity(&cube);
	render.addEntity(&metal);
	render.addEntity(&balloon);
	render.addDecor(&ground);
	render.addDecor(&tree);
	//render.addDecor(&wall);
	render.addLightSource(&handSpot);
	//render.addLightSource(&torch0);
	render.setDirectionalLight(&sunLight);

	//CubeMap sky("C:/Users/nicol/OneDrive/Documents/Graphismes/downloaded/skybox",
	//	std::vector<std::string>({"right.jpg", "left.jpg", "top.jpg", "bottom.jpg", "front.jpg", "back.jpg"}));
	//CubeMap sky();

	render.loadEnvironmentMap("C:/Users/nicol/OneDrive/Documents/Graphismes/downloaded/Ridgecrest_Road/Ridgecrest_Road/Ridgecrest_Road_Ref.hdr", 5000);
	//render.loadEnvironmentMap("C:/Users/nicol/OneDrive/Documents/Graphismes/downloaded/hdriMap/104_hdrmaps_com_free.hdr", 2000);
	//render.loadEnvironmentMap("C:/Users/nicol/OneDrive/Documents/Graphismes/downloaded/hdriMap/Milkyway_small.hdr", 5000);

	KeySwitch textIsDisplay(render.Window(), GLFW_KEY_F3, true);
	KeySwitch reload(render.Window(), GLFW_KEY_F5);

	render.setRenderingAlgorithm(Renderer::renderingAlg::physicaly_based_rendering);
	render.Window()->maximise();

	vec3 sunPosition;

	//render.Window()->maximise();
	render.Window()->setPos(1920/2, 0);
	render.Window()->setSize(1920/2, 1040);

	render.setAmbientStrength(.3f);

	// render the scene on this framebuffer
	uint32_t mainFBO;
	glGenFramebuffers(1, &mainFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

	uint32_t rendering;
	glGenTextures(1, &rendering);
	glBindTexture(GL_TEXTURE_2D, rendering);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rendering, 0);

	uint32_t depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Plane screen;
	Shader *screenShader = new Shader(NICO_SHADERS_PATH"screen.vert", NICO_SHADERS_PATH"postProcessing.frag");
	Key reloadPostProShader(render.Window(), GLFW_KEY_C);
	double timeOfLastReload = 0;


	//clouds geometry framebuffer
	uint32_t ppFBO;
	glGenFramebuffers(1, &ppFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, ppFBO);

	uint32_t fragDirMap;
	glGenTextures(1, &fragDirMap);
	glBindTexture(GL_TEXTURE_2D, fragDirMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8_SNORM, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fragDirMap, 0);

	uint32_t ppDepthMap;
	glGenTextures(1, &ppDepthMap);
	glBindTexture(GL_TEXTURE_2D, ppDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, ppDepthMap, 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	Object3d cloudsContainer(&cubeMesh, vec3(0, 2, 0), vec3(0.5, 2, 6));
	Shader* ppGeometryShader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_SHADERS_PATH"fragPos.frag");

	uint32_t noiseTexture = generate3dNoiseTexture(50, 50, 50, 10);

	do {
		render.clear();
		text.updateDisplay(render.Window());
			
		
		//lights and objects calculations :
		player.classicKeyboardControls(render.Window(), 5);
		player.classicMouseControls(render.Window(), 0.004f);

		handSpot.setPos(player.getPosition() + player.getRightDirection());
		handSpot.setDirection(player.getLook());
		
		if (render.Window()->Key(GLFW_KEY_G)) {
			render.removeDecor(&wall);
		}
		if (render.Window()->Key(GLFW_KEY_T)) {
			render.addDecor(&wall);
		}

		if (render.Window()->Key(GLFW_KEY_F11)) {
			render.Window()->setSize(1920, 1080);
			render.Window()->setPos(0,0);
		}

		light1Strength.setRelativeScale(vec2(0.1f));
		light1Strength.setPosition(vec2(100, 100));

		//sunLight.setColor(vec3((double)light1Strength));
		cloudsContainer.setScale(vec3((double)light1Strength, cloudsContainer.getPos().y, cloudsContainer.getPos().z));

		if (handSpotOn.justSwitch())
			if (handSpotOn)
				render.removeLightSource(&handSpot);
			else
				render.addLightSource(&handSpot);

		const double amp = 100;
		const double sunSpeed = 10;
		sunPosition = player.getPosition() + vec3(cos(render.Window()->getTime() / sunSpeed) * amp, amp, sin(render.Window()->getTime() / sunSpeed) * amp);
		sunLight.setDirection(- sunPosition + player.getPosition());

		cube.setPos(sunPosition);

		//render the scene in the main Framebuffer
		glEnable(GL_DEPTH_TEST);
		glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);

		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, render.Window()->getWidth(), render.Window()->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		
		render.update();
		render.frame(mainFBO);

		// render the geometry of the post-processings effects 
		glBindFramebuffer(GL_FRAMEBUFFER, ppFBO);
		glViewport(0, 0, 800, 600);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		player.sendToShader(ppGeometryShader);
		cloudsContainer.draw(ppGeometryShader);


		// render the result on the screen
		glBindFramebuffer(GL_FRAMEBUFFER, NULL);
		glViewport(0, 0, render.Window()->getWidth(), render.Window()->getHeight());
		
		glClear(GL_COLOR_BUFFER_BIT);
		glDisable(GL_DEPTH_TEST);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, rendering);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, render.Window()->getWidth(), render.Window()->getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);


		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, render.Window()->getWidth(), render.Window()->getHeight(), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, ppDepthMap);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, fragDirMap);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_3D, noiseTexture);

		screenShader->set("depthMap", 1);
		screenShader->set("ppDepthMap", 2);
		screenShader->set("fragDirMap", 3);
		screenShader->set("noise", 4);
		screenShader->set("viewPos", player.getPosition());
		screenShader->set("zNear", player.getZNear());
		screenShader->set("zFar", player.getZFar());
		screenShader->set("boxPos", cloudsContainer.getPos());
		screenShader->set("boxScale", cloudsContainer.getScale());
		screenShader->set("look", player.getLook());


		screen.draw(screenShader);

		render2d.frame();
		

		bill.draw(&billboardShader);

		timeOfLastReload += render.Window()->getDeltaTime();

		if (reloadPostProShader.isDown() or timeOfLastReload > 1) {
			screenShader = new Shader(NICO_SHADERS_PATH"screen.vert", NICO_SHADERS_PATH"postProcessing.frag");
			timeOfLastReload = 0;
		}
		if (reloadPostProShader.isDown()) {
			ppGeometryShader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_SHADERS_PATH"fragPos.frag");
		}
		reloadPostProShader.getState();

		//2d display :

		if (textIsDisplay)
		text.printLeftTop("fps : " + std::to_string(render.Window()->getFps()) + "  " + TextRenderer::textToPrint
			, 5, 5, 20);

		
	} while (!render.Window()->shouldClose() and !render.Window()->Key(GLFW_KEY_ESCAPE));

	delete screenShader;
	return 0;
}

int test2D() {
	Window win(4,3,true, 4);
	Renderer2d render(&win);
	TextRenderer text;

	NumberInput test(&win, vec2(120, 500), vec2(100, 30));
	test.setMultiplyColor(vec3(1,0,0));

	Editor editor(&win);

	render.addElement(&editor);

	TextInput editorWidth(&win, vec2(120, 50), vec2(100, 30));
	editorWidth.setMultiplyColor(vec3(0, 1, 0));
	editorWidth.setLayer(10);
	render.addElement(&editorWidth);

	Texture tex(NICO_TEXTURES_PATH"white.png");
	Object2d image(&tex, glm::vec2(100, 100), glm::vec2(100));
	image.setMultiplyColor(glm::vec3(0,0,1));
	image.setLayer(100);
	//render.addElement(&image);

	render.updateLayers();

	
	win.setSize(800, 400);

	glClearColor(0.56f, 1, 1, 1);

	do {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		TextRenderer::textToPrint = "";

		text.updateDisplay(&win);



		//editorWidth.setLayer((double) editorWidth);
		render.updateLayers();

		//editor.setRelativeWidth((double) editorWidth);

		if (win.Key(GLFW_KEY_F11)) {
			win.setSize(1920, 1080);
			win.setPos(0, 0);
		}
		render.frame();

		text.printLeftTop("fps : " + std::to_string(win.getFps()) + "  " + TextRenderer::textToPrint
			, 5, 5, 20, glm::vec4(1));
		win.endFrame();

		if (win.Key(GLFW_KEY_ESCAPE))
			win.setShouldClose(true);
	} while (!win.shouldClose());

	return 0;
}

int testJSON() {
	const std::string rawJson = R"({"Age": 20, "Name": "colin","light1": {"color":{"x":1, "y":0, "z":0}, "position" : {"x":2, "y":4, "z":-6} }})"; //

	Json::Value root;
	Json::String err;

	Json::CharReaderBuilder builder;
	const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

	bool worked = reader->parse(rawJson.c_str(), rawJson.c_str() + rawJson.length(), &root, &err);
	
	if (worked) {
		for (Json::Value::iterator it = root.begin(); it != root.end(); it++) {
			std::cout << it.name() << " : " << *it << " size : " << it->size() <<  std::endl;
			
		}

		std::cout << "\n" <<  root.toStyledString();
	}
	else { std::cout << "error : " << err;}
	return 0;
}

//int main() {
//	auto main = openGL;
//	return main();
//}