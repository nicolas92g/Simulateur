#pragma once
#include <nico.hpp>
#include <unordered_map>
#include <glad/glad.h>
#include <noise.h>
#include <chrono>

#define CHUNK_SIZE 256
#define MAX_RESOLUTION 10 //(8^2 = 256)

class Chunk : public nico::DrawableObject
{
public:
	Chunk(glm::ivec2 pos, uint32_t resolutionPowerOfTwo = 3);
	~Chunk();

	glm::ivec2 getPosition() const;

	void draw(nico::Shader* shader);

	void drawWater(nico::Shader* shader);

	void setResolution(uint8_t powerOfTwo);
	uint32_t getResolution() const;

	bool wasCreated() const;
	bool isReloadingRes() const;

	static nico::KeySwitch* lines;

	static void genWaterObject();
	static void destroyWaterObject();
	static uint32_t getNumberOfWorkingThreads();
	static float seaLevel;
	static float offsetBetweenSeaAndLand;

protected:
	void CalculateHeights();//~ < 100 ms for r = 7
	void CalculateVertices();//~ > 100 ms for r = 7
	void CalculateIndices();//2~3 ms for r = 7

	void sendData();// t<10 ms 

	static glm::vec3 calculateNormalVector(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3);
	static void calculateTangents(nico::Vertex* point);
	static void calculateTangentsAndBi(nico::Vertex* point);

	

	//chunk
	glm::ivec2 chunkGridPos;
	std::unordered_map<short, std::unordered_map<short, double>> heights;
	noise::module::Perlin noise;
	noise::module::RidgedMulti ridged;

	uint32_t resolution;//stock the current resolution of the chunk
	uint8_t ResPowerTwo;// 2**(this var) == resolution
	uint32_t maxPowerTwo;//stock the biggest resolution the chunk was(in the 2**this way)
	bool containWater;

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

	//opengl stuff
	uint32_t VAO;
	uint32_t VBO;
	uint32_t EBO;

	static uint32_t waterVAO;
	static uint32_t waterVBO;
	static uint32_t numberOfThreads;
};