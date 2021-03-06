#include "Land.h"

using namespace nico;
using namespace glm;



Land::Land(Renderer* r, Camera* cam) : reload(r->Window(), GLFW_KEY_X)//, trees(NICO_PATH"MODELISATION/tree/tree.obj")
{
	shader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_PATH"TERRAIN/terrain.frag");
	r->setShaderConstantsUniforms(this->shader);

	this->render = r;
	this->cam = cam;
	Chunk::genWaterObject();

	this->numberOfThreadMax = 1;

	this->renderDistance = 14;
	this->nearDistance = 2;
	this->middleDistance = 5;

	this->nearResolution = 8;
	this->middleResolution = 7;
	this->farResolution = 4;

#ifndef NDEBUG
	this->middleDistance = 4;
	this->nearResolution = 7;
	this->middleResolution = 6;
#endif // !NDEBUG


	//initRefractionSystem();
}

Land::~Land()
{
	delete shader;
	Chunk::destroyWaterObject();
	glDeleteFramebuffers(1, &refractionFBO);
	glDeleteTextures(1, &refractionColorMap);
	glDeleteTextures(1, &refractionDepthMap);
}

void Land::update()
{
	shader->set("seaLevel", Chunk::seaLevel);
	shader->set("ortho", (glm::mat4)glm::ortho(0, 800, 600, 0, -1, 1));
	bool needToReloadTrees = false;

	//reload shaders
	if (reload.isDown()) {
		shader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_PATH"TERRAIN/terrain.frag");
		render->setShaderConstantsUniforms(this->shader);
		shader->set("model", glm::translate(vec3(0)));
	}
	reload.getState();


	glm::ivec2 centralChunk = convertPlayerPosToChunkPos(cam->getPosition());

	if (this == nullptr)
		std::cout << "land::this = nullptr\n";
	
	//note too far chunks
	std::vector<glm::ivec2> chunksToUnload;

	for (auto& X : land)
	{
		for (auto& chunk : X.second)
		{
			if (this->getChunkDistance(centralChunk, chunk.second->getPosition()) > this->renderDistance) {
				chunksToUnload.push_back(chunk.second->getPosition());
			}
		}
	}

	//delete those chunks
	for (auto it = chunksToUnload.begin(); it != chunksToUnload.end(); it++)
	{
		land[it->x].erase(it->y);

		//remove empty chunks maps
		if (!land[it->x].size()) {
			land.erase(it->x);
		}
	}

	//if there is too much loading threads then stop everything
	if (Chunk::getNumberOfWorkingThreads() >= this->numberOfThreadMax) {
		return;
	}


	//check the first chunk
	if (!isLoaded(centralChunk)) {
		land[centralChunk.x][centralChunk.y] = std::make_unique<Chunk>(centralChunk, nearResolution, render);
		return;
	}
	if (land[centralChunk.x][centralChunk.y]->getResolution() != nearResolution) {
		land[centralChunk.x][centralChunk.y]->setResolution(nearResolution);
		needToReloadTrees = true;
		return;
	}

	//get the direction of the first chunk to load
	glm::ivec2 lookChunk = getChunkDirectionToLoad(cam->getLook());
	if (lookChunk == ivec2(0)) {
		lookChunk = ivec2(1, 0);
	}

	//search for a new chunk to load
	for (size_t i = 1; i <= renderDistance; i++)
	{
		ivec2 chunkToLoad = rotate(glm::radians(90.0f), lookChunk) * (int)i + lookChunk * (int)i + centralChunk;
		ivec2 directionOfTheLoading = ivec2(0);

		uint32_t resolutionOfChunks = nearResolution;

		if (i > nearDistance)
			resolutionOfChunks = middleResolution;
		if (i > middleDistance)
			resolutionOfChunks = farResolution;

		for (size_t j = 0; j < 8 * i; j++)
		{
			if (j < (i * 2))
				directionOfTheLoading = -rotate(glm::radians(90.0f), lookChunk);

			else if (j < (i * 4))
				directionOfTheLoading = -lookChunk;

			else if (j < (i * 6))
				directionOfTheLoading = rotate(glm::radians(90.0f), lookChunk);

			else if (j < (i * 8))
				directionOfTheLoading = lookChunk;

			else {
				return;
			}

			chunkToLoad += directionOfTheLoading;

			if (land.find(chunkToLoad.x) == land.end()) {
				land.insert(std::make_pair(chunkToLoad.x, 0));
			}

			if (!isLoaded(chunkToLoad)) {
				land[chunkToLoad.x][chunkToLoad.y] = std::make_shared<Chunk>(chunkToLoad, resolutionOfChunks, render);
				return;
			}

			if (land[chunkToLoad.x][chunkToLoad.y]->getResolution() != resolutionOfChunks) {
				land[chunkToLoad.x][chunkToLoad.y]->setResolution(resolutionOfChunks);
				if(resolutionOfChunks == nearResolution) needToReloadTrees = true;
				return;
			}
		}
	}
	
	//if (needToReloadTrees or true) {
	//	std::vector<glm::mat4> treesMatrices;
	//	treesMatrices.push_back(glm::translate(vec3(2555, 9, 5312)));
	//
	//	for (auto& X : land)
	//	{
	//		for (auto& chunk : X.second)
	//		{
	//			if (this->getChunkDistance(centralChunk, chunk.second->getPosition()) <= this->middleDistance) {
	//				treesMatrices.insert(treesMatrices.end(), chunk.second->getTrees().begin(), chunk.second->getTrees().end());
	//			}
	//		}
	//	}
	//	trees.setModels(treesMatrices);
	//	std::cout << "reload trees\n";
	//}
}

bool Land::isLoaded(glm::ivec2 chunk)
{
	auto it = land.find(chunk.x);
	if (it == land.end())
		return false;

	return it->second.find(chunk.y) != it->second.end();
}

uint32_t Land::getNumberOfLoadedChunks()
{
	uint32_t count(0);

	for (auto& x : land)
	{
		for (auto& chunk : x.second)
		{
			if (chunk.second->wasCreated())
				count++;
		}
	}
	return count;
}

void Land::draw(Shader* shader)
{
	//updateRefraction(shader);

	glBindFramebuffer(GL_FRAMEBUFFER, NULL);
	glViewport(0, 0, render->Window()->getWidth(), render->Window()->getHeight());

	cam->sendToShader(shader);
	
	preDrawCall(shader);
	drawLand(shader);
	drawWater(shader);

	glm::ivec2 p = convertPlayerPosToChunkPos(cam->getPosition());
	
}

void Land::draw()
{
	draw(this->shader);
}

void Land::useCamera(nico::Camera* cam)
{
	this->cam = cam;
}

void Land::setSeaLevel(float seaLevel)
{
	Chunk::seaLevel = seaLevel;
}

float Land::getSeaLevel() const
{
	return Chunk::seaLevel;
}

void Land::checkBadGeneration()
{
	for (auto& X : land)
	{
		for (auto& chunk : X.second) 
		{
			chunk.second->checkBadGeneration();
		}
	}
}

std::vector<Chunk*> Land::getNearestChunks(glm::vec3 pos)
{
	std::vector<Chunk*> chunks;

	ivec2 PlayerChunk = convertPlayerPosToChunkPos(pos);

	if (isLoaded(PlayerChunk))
		chunks.push_back(land[PlayerChunk.x][PlayerChunk.y].get());
	else
		return chunks;

	bool xPlus, zPlus;

	xPlus = pos.x > chunks[0]->getCenter().x;
	zPlus = pos.z > chunks[0]->getCenter().z;

	ivec2 C[3];

	if (xPlus) {
		C[0] = PlayerChunk + ivec2(1, 0);
	}
	else {
		C[0] = PlayerChunk + ivec2(-1, 0);
	}
	C[2].x = C[0].x;

	if (zPlus) {
		C[1] = PlayerChunk + ivec2(0, 1);
	}
	else {
		C[1] = PlayerChunk + ivec2(0, -1);
	}
	C[2].y = C[1].y;

	for (size_t i = 0; i < 3; i++)
	{
		if (isLoaded(C[i]))
			chunks.push_back(land[C[i].x][C[i].y].get());
	}

	return chunks;
}

std::vector<sphere>* Land::getHitbox(glm::vec3 pos)
{
	std::vector<Chunk*> chunks = getNearestChunks(pos);

	static std::vector<sphere> hitboxes;
	hitboxes.clear();

	for (size_t i = 0; i < chunks.size(); i++)
	{
		const std::vector<sphere>* h = chunks[i]->getHitbox();

		if(h != nullptr)
			hitboxes.insert(hitboxes.end(), h->begin(), h->end());
	}

	return &hitboxes;
}

void Land::initRefractionSystem()
{
	//initialize refraction textures resolution
	refractionRes = glm::ivec2(1920, 1080);

	//unbind all textures
	glBindTexture(GL_TEXTURE_2D, NULL);

	//create the FBO
	glGenFramebuffers(1, &refractionFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);

	// generate color texture
	glGenTextures(1, &refractionColorMap);
	glBindTexture(GL_TEXTURE_2D, refractionColorMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, refractionRes.x, refractionRes.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, refractionColorMap, 0);

	// generate depth texture
	glGenTextures(1, &refractionDepthMap);
	glBindTexture(GL_TEXTURE_2D, refractionDepthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, refractionRes.x, refractionRes.y, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, refractionDepthMap, 0);

	//check framebuffer 
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "error : refraction framebuffer is not complete" << std::endl;

	//unbind FBO
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Land::updateRefraction(Shader* shader)
{
	//remove all the land that is above the water of the rendering 
	glEnable(GL_CLIP_DISTANCE0);
	shader->set("clipPlane", vec4(0, -1, 0, Chunk::seaLevel));

	//bind refraction FBO
	glBindFramebuffer(GL_FRAMEBUFFER, refractionFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	glViewport(0, 0, refractionRes.x, refractionRes.y);

	//draw only the land
	preDrawCall(shader);
	drawLand(shader);


	//reset the rendering as normal 
	shader->set("clipPlane", vec4(0, 1, 0, 100000));
	glDisable(GL_CLIP_DISTANCE0);
}

void Land::preDrawCall(Shader* shader)
{
	render->prepareShaderBeforeDraw(shader);
	render->sendLightsToShader(shader);
	shader->set("time", glfwGetTime());
	shader->set("seaLevel", Chunk::seaLevel);

	glActiveTexture(GL_TEXTURE10);
	glBindTexture(GL_TEXTURE_2D, refractionColorMap);
	shader->set("refractionColor", 10);

	glActiveTexture(GL_TEXTURE11);
	glBindTexture(GL_TEXTURE_2D, refractionDepthMap);
	shader->set("refractionDepth", 11);

	static const glm::mat4 model = glm::translate(vec3(0));
	shader->set("model", model);

	glDepthFunc(GL_LESS);
}

void Land::drawLand(Shader* shader)
{
	for (auto& X : land)
	{
		for (auto& chunk : X.second)
		{
			chunk.second->draw(shader);
		}
	}
	//trees.draw(shader);

	//glm::ivec2 p = convertPlayerPosToChunkPos(cam->getPosition());

	//sphere::affichage.insert(sphere::affichage.end(), land[p.x][p.y]->getHitbox()->begin(), land[p.x][p.y]->getHitbox()->end());
	//sphere::affichage = *land[p.x][p.y]->getHitbox();
}

void Land::drawWater(Shader* shader)
{
	bool cullFace = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);

	for (auto& X : land)
	{
		for (auto& chunk : X.second)
		{
			chunk.second->drawWater(shader);
		}
	}

	if (cullFace)
		glEnable(GL_CULL_FACE);
}

glm::ivec2 Land::convertPlayerPosToChunkPos(glm::vec3 pos)
{
	glm::ivec2 ret;
	ret.x = int(pos.x / (float)CHUNK_SIZE);
	ret.y = int(pos.z / (float)CHUNK_SIZE);

	if (pos.x < 0) {
		ret.x--;
	}
	if (pos.z < 0) {
		ret.y--;
	}
	return ret;
}

uint16_t Land::getChunkDistance(glm::ivec2 centralChunk, glm::ivec2 chunk)
{
	const glm::ivec2 offset = glm::abs(chunk - centralChunk);

	return std::max(offset.x, offset.y);
}

glm::ivec2 Land::getChunkDirectionToLoad(glm::vec3 look)
{
	glm::ivec2 ret;
	look = normalize(look);
	if (abs(look.x) > abs(look.z)) {
		ret = normalize(vec2((float)look.x, 0.0f));
	}
	else if (abs(look.x) < abs(look.z)) {
		ret = normalize(vec2(0.0f, (float)look.z));
	}
	else {
		ret = normalize(vec2(0.0f, (float)look.z + 0.05f));
	}

	return ret;
}

glm::ivec2 Land::rotate(float angle, glm::ivec2 dir)
{
	vec3 dir3 = vec4((float)dir.x, 0.0f, (float)dir.y, 1.0f) * glm::rotate(angle, vec3(0.0f, 1.0f, 0.0f));
	if ((int)dir3.x + (int)dir3.z != 0) {
		dir3 = normalize(dir3);
	}
	return ivec2((int)dir3.x, (int)dir3.z);
}
