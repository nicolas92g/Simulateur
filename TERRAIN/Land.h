#pragma once
#include <nico.hpp>
#include "Chunk.h"
#include <unordered_map>


class Land : public nico::DrawableObject
{
public:
	Land(nico::Renderer* r, nico::Camera* cam);
	~Land();

	void update();
	bool isLoaded(glm::ivec2 chunk);

	void draw(nico::Shader* shader);
	void draw();

	void useCamera(nico::Camera* cam);

	void setSeaLevel(float seaLevel);
	float getSeaLevel() const;

	std::vector<Chunk*> getNearestChunks(unsigned int number, glm::vec3 pos);

protected:
	

	nico::Shader* shader;
	nico::Key reload;

	nico::Renderer* render;
	nico::Camera* cam;
	uint16_t numberOfThreadMax;

	uint32_t renderDistance;
	uint32_t nearDistance;
	uint32_t middleDistance;

	uint32_t farResolution;
	uint32_t middleResolution;
	uint32_t nearResolution;

	std::unordered_map<int, std::unordered_map<int, std::shared_ptr<Chunk>>> land;

	//water
	glm::ivec2 refractionRes;
	uint32_t refractionFBO;
	uint32_t refractionDepthMap;
	uint32_t refractionColorMap;
	void initRefractionSystem();
	void updateRefraction();

	
	//utils functions

	void preDrawCall(nico::Shader* shader);
	void drawLand(nico::Shader* shader);
	void drawWater(nico::Shader* shader);
	glm::ivec2 convertPlayerPosToChunkPos(glm::vec3 pos);
	uint16_t getChunkDistance(glm::ivec2 centralChunk, glm::ivec2 chunk);
	glm::ivec2 getChunkDirectionToLoad(glm::vec3 look);
	glm::ivec2 rotate(float angle, glm::ivec2 dir);
};

