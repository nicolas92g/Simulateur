#include "Land.h"

using namespace nico;
using namespace glm;

float Chunk::seaLevel = 1;

Land::Land(Renderer* r, Camera* cam) : reload(r->Window(), GLFW_KEY_X)
{
	shader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_PATH"TERRAIN/terrain.frag");
	r->setShaderConstantsUniforms(this->shader);

	this->render = r;
	this->cam = cam;
	Chunk::genWaterObject();

	this->numberOfThreadMax = 1;
	this->renderDistance = 10;
	this->nearResolution = 8;
	this->middleResolution = 6;
	this->farResolution = 4;
}

Land::~Land()
{
	delete shader;
	Chunk::destroyWaterObject();
}

void Land::update()
{

	//reload shaders
	if (reload.isDown()) {
		shader = new Shader(NICO_SHADERS_PATH"default.vert", NICO_PATH"TERRAIN/terrain.frag");
		render->setShaderConstantsUniforms(this->shader);
		shader->set("model", glm::translate(vec3(0)));
	}
	reload.getState();


	glm::ivec2 centralChunk = convertPlayerPosToChunkPos(cam->getPosition());
	
	//note too far chunks
	std::list<glm::ivec2> chunksToUnload;
	for (auto itx = land.begin(); itx != land.end(); itx++)
	{
		for (auto ity = itx->second.begin(); ity != itx->second.end(); ity++)
		{
			if (this->getChunkDistance(centralChunk, ity->second->getPosition()) > this->renderDistance) {
				chunksToUnload.push_back(ity->second->getPosition());
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

	//create new chunk
	/*for (int i = -1; i <= 1; i++)
	{
		for (int j = -1; j <= 1; j++)
		{
			const ivec2 c = centralChunk + ivec2(i, j);

			if (!isLoaded(c))
				land[c.x][c.y] = std::make_unique<Chunk>(c);
		}	
	}*/
	//if there is too much loading threads then stop everything
	if (Chunk::getNumberOfWorkingThreads() >= this->numberOfThreadMax) {
		return;
	}

	//check the first chunk
	if (!isLoaded(centralChunk)) {
		land[centralChunk.x][centralChunk.y] = std::make_unique<Chunk>(centralChunk);
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
		if (i > 3)
			resolutionOfChunks = middleResolution;
		if (i > 5)
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
				break;
			}

			chunkToLoad += directionOfTheLoading;

			if (!isLoaded(chunkToLoad)) {
				land[chunkToLoad.x][chunkToLoad.y] = std::make_unique<Chunk>(chunkToLoad, resolutionOfChunks);
				return;
			}

			if (land[chunkToLoad.x][chunkToLoad.y]->getResolution() != resolutionOfChunks) {
				land[chunkToLoad.x][chunkToLoad.y]->setResolution(resolutionOfChunks);
				return;
			}
		}
	}
	

}

bool Land::isLoaded(glm::ivec2 chunk)
{
	auto it = land.find(chunk.x);
	if (it == land.end())
		return false;

	return it->second.find(chunk.y) != it->second.end();
}

void Land::draw(Shader* shader)
{
	cam->sendToShader(shader);
	render->prepareShaderBeforeDraw(shader);
	render->sendLightsToShader(shader);
	shader->set<float>("time", glfwGetTime());
	shader->set("seaLevel", seaLevel);

	glDepthFunc(GL_LESS);

	shader->set("model", glm::translate(vec3(0)));
	for (auto y = land.begin(); y != land.end(); y++)
	{
		for (auto x = y->second.begin(); x != y->second.end(); x++)
		{
			x->second->draw(shader);
		}
	}

	bool cullFace = glIsEnabled(GL_CULL_FACE);
	glDisable(GL_CULL_FACE);
	

	for (auto y = land.begin(); y != land.end(); y++)
	{
		for (auto x = y->second.begin(); x != y->second.end(); x++)
		{
			x->second->drawWater(shader);
		}
	}

	if (cullFace)
		glEnable(GL_CULL_FACE);
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
	this->seaLevel = seaLevel;
}

float Land::getSeaLevel() const
{
	return seaLevel;
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
