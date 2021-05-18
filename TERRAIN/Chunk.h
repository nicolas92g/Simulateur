#pragma once
#include <nico.hpp>
#include <unordered_map>
#include <glad/glad.h>
#include <noise.h>
#include <chrono>
#include <SOURCE/Deplacement.h>

#define CHUNK_SIZE 256
#define MAX_RESOLUTION 10 //(8^2 = 256)

class Chunk : public nico::DrawableObject
{
public:
	Chunk(glm::ivec2 pos, uint32_t resolutionPowerOfTwo, nico::Renderer* render);
	~Chunk();

	glm::ivec2 getPosition() const;
	glm::vec3 getCenter();

	void draw(nico::Shader* shader);

	void drawWater(nico::Shader* shader);

	void setResolution(uint8_t powerOfTwo);
	uint32_t getResolution() const;

	bool wasCreated() const;
	bool isReloadingRes() const;
	std::vector<sphere>* getHitbox();

	void CalculateHitbox();
	void checkBadGeneration();


	static nico::KeySwitch* lines;

	static void genWaterObject();
	static void destroyWaterObject();
	static uint32_t getNumberOfWorkingThreads();
	static float seaLevel;
	static float offsetBetweenSeaAndLand;

protected:
	void CalculateHeights();
	void CalculateVertices();
	void CalculateIndices();

	void sendData();

	static glm::vec3 calculateNormalVector(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	static void calculateTangents(nico::Vertex* point);
	static void calculateTangentsAndBi(nico::Vertex* point);
	static double heightFunction(double X, double Y);

	

	//chunk
	glm::ivec2 chunkGridPos;
	std::unordered_map<int, std::unordered_map<int, double>> heights;
	static noise::module::Perlin noise;
	static noise::module::RidgedMulti ridged;

	uint32_t resolution;//stock the current resolution of the chunk
	uint8_t ResPowerTwo;// 2**(this var) == resolution
	uint32_t maxPowerTwo;//stock the biggest resolution the chunk was(in the 2**this way)
	nico::Renderer* render;
	bool containWater;

	//std::vector<glm::mat4> trees;

	static nico::Texture* normal;//land normal map
	static nico::Texture* waterBump;//water normal map

	//multi-threading:
	bool isCreated;//is true when the VAO has been filled once
	bool isReloading;//is true when a thread is currently regenerating the chunk

	static void createChunkData(Chunk* chunk);
	std::thread* loadingThread;
	//this is the function pointer used to draw :
	void(*DrawFunc)(nico::Shader*, Chunk*);
	static void preCreatedDrawCall(nico::Shader* shader, Chunk* chunk);
	static void drawCall(nico::Shader* shader, Chunk* chunk);
	static void generatingDrawCall(nico::Shader* shader, Chunk* chunk);

	//data of the chunk
	std::vector<nico::Vertex> vertices;
	std::vector<uint32_t> indices;
	std::vector<sphere> hitbox;

	//opengl stuff
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	static uint32_t waterVAO;
	static uint32_t waterVBO;
	static uint32_t numberOfThreads;
};
