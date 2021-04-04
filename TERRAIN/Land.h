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

protected:
	

	nico::Shader* shader;
	nico::Key reload;

	nico::Renderer* render;
	nico::Camera* cam;
	uint16_t numberOfThreadMax;

	float seaLevel;
	uint32_t renderDistance;
	uint32_t farResolution;
	uint32_t middleResolution;
	uint32_t nearResolution;

	std::unordered_map<int, std::map<int, std::unique_ptr<Chunk>>> land;

	glm::ivec2 convertPlayerPosToChunkPos(glm::vec3 pos);
	uint16_t getChunkDistance(glm::ivec2 centralChunk, glm::ivec2 chunk);
	glm::ivec2 getChunkDirectionToLoad(glm::vec3 look);
	glm::ivec2 rotate(float angle, glm::ivec2 dir);
};

