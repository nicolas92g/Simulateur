#include "Chunk.h"

#define NUMBER_OF_HEIGHTS resolution + 3 // two more vertices for normals calculations
#define NUMBER_OF_VERTICES resolution + 1 // this is the number of vertices per chunk's edge

float Chunk::offsetBetweenSeaAndLand = 0.5f;
float Chunk::seaLevel = 1.0f;

using namespace nico;
using namespace glm;

noise::module::Perlin Chunk::noise;
noise::module::RidgedMulti Chunk::ridged;

Chunk::Chunk(ivec2 pos, uint32_t res, nico::Renderer* render)
{
	this->chunkGridPos = pos;
	this->ResPowerTwo = res;
	this->resolution = pow(2, ResPowerTwo);
	this->maxPowerTwo = 0;
	this->VAO = NULL;
	this->VBO = NULL;
	this->EBO = NULL;
	this->isReloading = false;
	this->render = render;

	noise.SetSeed(-123);
	this->isCreated = false;
	this->DrawFunc = preCreatedDrawCall;
	this->loadingThread = new std::thread(&Chunk::createChunkData, this);
	this->loadingThread->detach();
	this->containWater = true;
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

glm::vec3 Chunk::getCenter()
{
	glm::vec3 ret;
	ret.x = chunkGridPos.x * CHUNK_SIZE + CHUNK_SIZE * .5f;
	ret.z = chunkGridPos.y * CHUNK_SIZE + CHUNK_SIZE * .5f;
	ret.y = 0;
	return ret;
}

void Chunk::draw(nico::Shader* shader) {
	
	DrawFunc(shader, this);
}

void Chunk::drawWater(nico::Shader* shader)
{
	if (!containWater)
		return;

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

std::vector<sphere>* Chunk::getHitbox()
{
	if(isCreated)
		return &hitbox;
	return nullptr;
}

//std::vector<glm::mat4>& Chunk::getTrees()
//{
//	return trees;
//}

void Chunk::CalculateHeights()
{
	//fill vertical Pos with one more pos in each direction to determine normals
	const uint32_t offset = 1;// pow(2, MAX_RESOLUTION - ResPowerTwo);
	const double resize = 1.0 / ((double)resolution / (double)CHUNK_SIZE);
	const double xOffset = (double)chunkGridPos.x * (double)CHUNK_SIZE;
	const double yOffset = (double)chunkGridPos.y * (double)CHUNK_SIZE;
	
	containWater = false;
	//uint32_t numberOfTreesGenerated(0);
	//trees.resize((NUMBER_OF_HEIGHTS) * (NUMBER_OF_HEIGHTS));

	for (uint32_t i = 0; i < NUMBER_OF_HEIGHTS; i ++)
	{
		for (uint32_t j = 0; j < NUMBER_OF_HEIGHTS; j++)
		{
			const uint32_t x = i * offset;
			const uint32_t y = j * offset;
			const double X = (double(((int)i - 1) * resize) + xOffset);
			const double Y = (double(((int)j - 1) * resize) + yOffset);

			//the array is filled in a way that we can add some heights between previous values
			heights[x][y] = heightFunction(X, Y);

			//if (ResPowerTwo > 6) {
			//	if (noise.GetValue(X * 0.2, Y * 0.2, 1) - 0.5 > noise.GetValue(X * 0.01, Y * 0.01, 1)) {
			//		trees[numberOfTreesGenerated] = glm::translate(vec3(X, heights[x][y], Y));
			//		numberOfTreesGenerated++;
			//	}
			//}
			//check if the chunk contain some underwater vertices
			if (heights[x][y] < seaLevel)
				containWater = true;
		}
	}

	//trees.resize(numberOfTreesGenerated);
}

void Chunk::CalculateVertices()
{
	const float offset = 1;// pow(2, MAX_RESOLUTION - ResPowerTwo);
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

			vertex.x = vertex.x * resize;
			vertex.z = vertex.z * resize;

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
			
			vertice.positions.x += xOffset;
			vertice.positions.z += yOffset;

			vertice.normals = normal1 + normal2 + normal3 + normal4;
			// tangent and bitangents
			calculateTangents(&vertice);

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

double Chunk::heightFunction(double X, double Y)
{
	float A = std::max(0.25, noise.GetValue(X * 0.0002, 0, Y * 0.0002)) * 1.2;
	A *= (std::max(0.8, noise.GetValue(X * 0.00015, 0, Y * 0.00015)) * 5) - 3;

	double H = noise.GetValue(X * 0.0003, Y * 0.0003, 0) * 100 +
		(noise.GetValue(X * 0.005, 0, Y * 0.005)) * 10.0 +
		ridged.GetValue(X * 0.0003, Y * 0.0003, 4) * 170 * A;
	H *= 1.5;

	//avoid Z-fighting
	//if (H > seaLevel) {
	//	if (H < ((double)seaLevel + offsetBetweenSeaAndLand)) {
	//		H = ((double)seaLevel + offsetBetweenSeaAndLand);
	//	}
	//}
	//else {
	//	if (H > ((double)seaLevel - offsetBetweenSeaAndLand)) {
	//		H = ((double)seaLevel - offsetBetweenSeaAndLand);
	//	}
	//}

	return H;
}

void Chunk::calculateTangents(nico::Vertex* point)
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
}

void Chunk::createChunkData(Chunk* chunk)
{
	Chunk::numberOfThreads++;

	chunk->CalculateHeights();
	chunk->CalculateVertices();
	chunk->CalculateIndices();
	chunk->CalculateHitbox();

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
	if (!chunk->isCreated)
		return;

	shader->use();
	glBindVertexArray(chunk->VAO);

	shader->set("waterBump", 0);
	glActiveTexture(GL_TEXTURE0);
	Chunk::waterBump->bind();

	shader->set("normalMap", 1);
	glActiveTexture(GL_TEXTURE1);
	Chunk::normal->bind();

	glDrawElements(GL_TRIANGLES, chunk->resolution * chunk->resolution * 6, GL_UNSIGNED_INT, (void*)0);
}

void Chunk::generatingDrawCall(nico::Shader* shader, Chunk* chunk)
{
if (!chunk->isReloading) {
	chunk->sendData();
	chunk->DrawFunc = &Chunk::drawCall;
	delete chunk->loadingThread;
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
			indices[i] = (x + (y * (resolution + 1)));
			indices[i + 1] = ((x + 1) + (y * (resolution + 1)));
			indices[i + 2] = (x + ((y + 1) * (resolution + 1)));
			indices[i + 3] = (x + ((y + 1) * (resolution + 1)));
			indices[i + 4] = ((x + 1) + (y * (resolution + 1)));
			indices[i + 5] = ((x + 1) + ((y + 1) * (resolution + 1)));
		}
	}
}

void Chunk::CalculateHitbox() {
	hitbox.clear();
	hitbox.resize(vertices.size());

	const float rayon = CHUNK_SIZE / resolution;

	for (uint32_t i = 0; i < vertices.size(); i++) {
		hitbox[i] = { vertices[i].positions - rayon * 0.9f, rayon };
	}
}

void Chunk::checkBadGeneration()
{
	const uint32_t offset = 1;// pow(2, MAX_RESOLUTION - ResPowerTwo);
	const double resize = 1.0 / ((double)resolution / (double)CHUNK_SIZE);
	const double xOffset = (double)chunkGridPos.x * (double)CHUNK_SIZE;
	const double yOffset = (double)chunkGridPos.y * (double)CHUNK_SIZE;

	for (uint32_t i = 0; i < NUMBER_OF_HEIGHTS - 4; i += 4)
	{
		for (uint32_t j = 0; j < NUMBER_OF_HEIGHTS - 4; j += 4)
		{
			const uint32_t x = i * offset;
			const uint32_t y = j * offset;
			const double X = (double(((int)i - 1) * resize) + xOffset);
			const double Y = (double(((int)j - 1) * resize) + yOffset);

			if (heights[x][y] != heightFunction(X, Y))
				this->setResolution(ResPowerTwo);
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
