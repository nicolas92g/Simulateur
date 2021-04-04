#include "Chunk.h"

#define NUMBER_OF_HEIGHTS resolution + 3 // two more vertices for normals calculations
#define NUMBER_OF_VERTICES resolution + 1 // this is the number of vertices per chunk's edge

float Chunk::offsetBetweenSeaAndLand = 0.01f;

using namespace nico;
using namespace glm;


Chunk::Chunk(ivec2 pos, uint32_t res)
{
	this->chunkGridPos = pos;
	this->ResPowerTwo = res;
	this->resolution = pow(2, ResPowerTwo);
	this->maxPowerTwo = 0;
	this->seaLevel = 0.9f;
	this->VAO = NULL;
	this->VBO = NULL;
	this->EBO = NULL;
	this->isReloading = false;

	this->isCreated = false;
	this->DrawFunc = preCreatedDrawCall;
	this->loadingThread = new std::thread(&Chunk::createChunkData, this);
	this->loadingThread->detach();

	noise.SetSeed(8768768);
}

Chunk::~Chunk(){
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

glm::ivec2 Chunk::getPosition() const
{
	return this->chunkGridPos;
}

void Chunk::draw(nico::Shader* shader) {
	
	DrawFunc(shader, this);
}

void Chunk::drawWater(nico::Shader* shader)
{
	shader->set("water", true);

	shader->set("normalMap", 3);
	glActiveTexture(GL_TEXTURE3);
	waterBump->bind();

	shader->set("model", 
		glm::translate(vec3( chunkGridPos.x * CHUNK_SIZE, Chunk::seaLevel, chunkGridPos.y * CHUNK_SIZE)) *
		glm::scale(vec3(CHUNK_SIZE)));

	glBindVertexArray(waterVAO);

	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	shader->set("water", false);
}

void Chunk::setResolution(uint8_t powerOfTwo)
{
	this->ResPowerTwo = powerOfTwo;
	this->resolution = pow(2,powerOfTwo);

	maxPowerTwo = std::max<uint32_t>(maxPowerTwo, ResPowerTwo);
	
	this->isReloading = true;

	this->DrawFunc = &generatingDrawCall;
	this->loadingThread = new std::thread(&Chunk::createChunkData, this);
	this->loadingThread->detach();
}

uint32_t Chunk::getResolution() const
{
	return ResPowerTwo;
}

bool Chunk::wasCreated() const
{
	return isCreated;
}

bool Chunk::isReloadingRes() const
{
	return isReloading;
}

void Chunk::CalculateHeights()
{
	//fill vertical Pos with one more pos in each direction to determine normals
	const uint32_t offset = pow(2, MAX_RESOLUTION - ResPowerTwo);
	const double resize = 1.0 / ((double)resolution / (double)CHUNK_SIZE);
	const double xOffset = (double)chunkGridPos.x * (double)CHUNK_SIZE;
	const double yOffset = (double)chunkGridPos.y * (double)CHUNK_SIZE;
	

	for (uint32_t i = 0; i < NUMBER_OF_HEIGHTS; i ++)
	{
		for (uint32_t j = 0; j < NUMBER_OF_HEIGHTS; j++)
		{
			const uint32_t x = i * offset;
			const uint32_t y = j * offset;
			const double X = (double(((int)i - 1) * resize) + xOffset);
			const double Y = (double(((int)j - 1) * resize) + yOffset);

			//the array is filled in a way that we can add some heights between previous values

			const float A = (noise.GetValue(X * 0.001, 0, Y * 0.001)) * 60.0f;
			heights[x][y] = A +
				abs(noise.GetValue(X * 0.01, 0, Y * 0.01)) +
				(noise.GetValue(X * 0.005, 0, Y * 0.005)) * 10.0;


			//avoid Z-fighting
			if (heights[x][y] > seaLevel) {
				if (heights[x][y] < (seaLevel + offsetBetweenSeaAndLand)) {
					heights[x][y] = (seaLevel + offsetBetweenSeaAndLand);
				}
			}
			else {
				if (heights[x][y] > (seaLevel - offsetBetweenSeaAndLand)) {
					heights[x][y] = (seaLevel - offsetBetweenSeaAndLand);
				}
			}
		}
	}
}

void Chunk::CalculateVertices()
{
	const float offset = pow(2, MAX_RESOLUTION - ResPowerTwo);
	const uint32_t length = NUMBER_OF_VERTICES;
	const float resize = 1.0f / ((float)resolution / (float)CHUNK_SIZE);
	const float xOffset = chunkGridPos.x * CHUNK_SIZE;
	const float yOffset = chunkGridPos.y * CHUNK_SIZE;

	vertices.clear();
	vertices.resize((size_t)length * (size_t)length);

	for (uint16_t x = 0; x < length; x++) {
		for (uint16_t y = 0; y < length; y++)
		{
			glm::vec3 vertex = glm::vec3(x, heights[(x + 1) * offset ][(y + 1) * offset], y);

			vertex.x = vertex.x * resize + xOffset;
			vertex.z = vertex.z * resize + yOffset;

						//location
			Vertex vertice(vertex);
			//uvs
			vertice.texturesUv = vec2(vertex.x * resize, vertex.z * resize);
			//normals
			glm::vec3 vertex1 = glm::vec3((int)x - 1, heights[x * offset][(y + 1) * offset], y     );
			glm::vec3 vertex2 = glm::vec3(x    , heights[(x + 1) * offset][(y + 2) * offset], y + 1);
			glm::vec3 vertex3 = glm::vec3(x + 1, heights[(x + 2) * offset][(y + 1) * offset], y    );
			glm::vec3 vertex4 = glm::vec3(x    , heights[(x + 1) * offset][(y    ) * offset], (int)y - 1);

			glm::vec3 normal1 = calculateNormalVector(vertex, vertex1, vertex2);
			glm::vec3 normal2 = calculateNormalVector(vertex, vertex2, vertex3);
			glm::vec3 normal3 = calculateNormalVector(vertex, vertex3, vertex4);
			glm::vec3 normal4 = calculateNormalVector(vertex, vertex4, vertex1);

			vertice.normals = normal1 + normal2 + normal3 + normal4;
			// tangent and bitangents
			calculateTangentsAndBi(&vertice);

			vertices[(((size_t)x * length) + (size_t)y)] = vertice;
		}
	}
}

glm::vec3 Chunk::calculateNormalVector(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3)
{
	//this function use the algorithm find on 
	//https://www.khronos.org/opengl/wiki/Calculating_a_Surface_Normal

	glm::vec3 U = p2 - p1;
	glm::vec3 V = p3 - p1;

	return (glm::vec3(U.y * V.z - U.z * V.y, U.z * V.x - U.x * V.z, U.x * V.y - U.y * V.x));
}

void Chunk::calculateTangentsAndBi(nico::Vertex* point)
{
	glm::vec3 c1 = glm::cross(point->normals, glm::vec3(0.0, 0.0, 1.0));
	glm::vec3 c2 = glm::cross(point->normals, glm::vec3(0.0, 1.0, 0.0));

	if (glm::length(c1) > glm::length(c2))
	{
		point->tangents = c1;
	}
	else
	{
		point->tangents = c2;
	}

	point->tangents = glm::normalize(point->tangents);
	point->bitangents = glm::cross(point->tangents, point->normals);
}

void Chunk::createChunkData(Chunk* chunk)
{

	Chunk::numberOfThreads++;
	//auto start = std::chrono::high_resolution_clock::now();
	chunk->CalculateHeights();

	//auto stop = std::chrono::high_resolution_clock::now();
	//auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//std::cout << "\nCalculateHeights() a pris :" << (float)duration.count() / 1000.0f << "  ms\n";

	//start = std::chrono::high_resolution_clock::now();
	chunk->CalculateVertices();

	//stop = std::chrono::high_resolution_clock::now();
	//duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
	//std::cout << "CalculateVertices() a pris :" << (float)duration.count() / 1000.0f << "  ms\n\n";

	chunk->CalculateIndices();


	chunk->isCreated = true;
	chunk->isReloading = false;
	Chunk::numberOfThreads--;
}

void Chunk::preCreatedDrawCall(Shader* shader, Chunk* chunk)
{
	//test if the chunk is created
	//and then send the vertices to the Graphic card
	//and replace the draw call with the real draw call not this function
	if (chunk->isCreated) {
		chunk->sendData();
		chunk->DrawFunc = &drawCall;
		delete chunk->loadingThread;
	}
}

void Chunk::drawCall(Shader* shader, Chunk* chunk)
{
	shader->use();
	glBindVertexArray(chunk->VAO);

	shader->set("normalMap", 3);
	glActiveTexture(GL_TEXTURE3);
	chunk->normal->bind();

	GLenum rendering;

	if (*lines)
		rendering = GL_TRIANGLES;
	else
		rendering = GL_LINES;

	glDrawElements(rendering, chunk->resolution * chunk->resolution * 6, GL_UNSIGNED_INT, (void*)0);
}

void Chunk::generatingDrawCall(nico::Shader* shader, Chunk* chunk)
{
	if (!chunk->isReloading) {
		chunk->sendData();
		chunk->DrawFunc = &Chunk::drawCall;
		delete chunk->loadingThread;
		return;
	}

	drawCall(shader, chunk);
}

void Chunk::genWaterObject()
{
	std::vector<float> vertices = {
		// positions	//normals	// texture Coords
		0.0f, 0, 1.0f,	0, 1, 0,	0.0f, 1.0f,		1, 0, 0,	0, 0, 1,
		1.0f, 0, 1.0f,	0, 1, 0,	1.0f, 1.0f,		1, 0, 0,	0, 0, 1,
		0.0f, 0, 0.0f,	0, 1, 0,	0.0f, 0.0f,		1, 0, 0,	0, 0, 1,
		1.0f, 0, 0.0f,	0, 1, 0,	1.0f, 0.0f,		1, 0, 0,	0, 0, 1,
	};

	glGenVertexArrays(1, &waterVAO);
	glBindVertexArray(waterVAO);

	glGenBuffers(1, &waterVBO);
	glBindBuffer(GL_ARRAY_BUFFER, waterVBO);
	
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(float) * 14, (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(float) * 14, (void*)(3 * sizeof(float)));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, false, sizeof(float) * 14, (void*)(6 * sizeof(float)));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(float) * 14, (void*)(8 * sizeof(float)));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(float) * 14, (void*)(11 * sizeof(float)));


	normal = new Texture(NICO_PATH"TERRAIN/normal.png");
	waterBump = new Texture(NICO_PATH"TERRAIN/water.jpg");
}

void Chunk::destroyWaterObject()
{
	glDeleteBuffers(1, &waterVBO);
	glDeleteVertexArrays(1, &waterVAO);
	delete normal;
	delete waterBump;
}

uint32_t Chunk::getNumberOfWorkingThreads()
{
	return Chunk::numberOfThreads;
}

void Chunk::CalculateIndices()
{
	indices.clear();
	indices.resize(resolution * resolution * 6);

	for (uint16_t x = 0; x < resolution; x++) {
		for (uint16_t y = 0; y < resolution; y++)
		{
			uint32_t i = (x * resolution + y) * 6;
			indices[ i     ] = (x + (y * (resolution + 1)));
			indices[ i + 1 ] = ((x + 1) + (y * (resolution + 1)));
			indices[ i + 2 ] = (x + ((y + 1) * (resolution + 1)));
			indices[ i + 3 ] = (x + ((y + 1) * (resolution + 1)));
			indices[ i + 4 ] = ((x + 1) + (y * (resolution + 1)));
			indices[ i + 5 ] = ((x + 1) + ((y + 1) * (resolution + 1)));
		}
	}
}

void Chunk::sendData()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(nico::Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(nico::Vertex), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(nico::Vertex), (void*)offsetof(Vertex, normals));

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, false, sizeof(nico::Vertex), (void*)offsetof(Vertex, texturesUv));

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, false, sizeof(nico::Vertex), (void*)offsetof(Vertex, tangents));

	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, false, sizeof(nico::Vertex), (void*)offsetof(Vertex, bitangents));

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(uint32_t), &indices[0], GL_STATIC_DRAW);

	glBindVertexArray(0);
}

nico::KeySwitch* Chunk::lines;

uint32_t Chunk::waterVAO;
uint32_t Chunk::waterVBO;

nico::Texture* Chunk::normal;
nico::Texture* Chunk::waterBump;

uint32_t Chunk::numberOfThreads = 0;
