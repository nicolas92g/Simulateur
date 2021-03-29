#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <fstream>

void nico::Texture::loadTexture(const char* image, texParameters param)
{

	stbi_set_flip_vertically_on_load(false);

	unsigned char* data = stbi_load(image, &width, &height, &nbrChannels, STBI_rgb_alpha);
	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, param.wrap_s);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, param.wrap_t);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, param.min_filter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, param.mag_filter);

	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to loaded textures : " << image << "\n";
	}
	stbi_image_free(data);
}

nico::Texture::Texture(const char* imageFilePath, texParameters param, aiTextureType type) : texture(0)
{
	loadTexture(imageFilePath,  param);
	this->type = type;
	path = imageFilePath;
}


void nico::Texture::useTexture(unsigned int channel)
{
	if (texture < 32) {
		glActiveTexture(GL_TEXTURE0 + channel);
		texture = channel;
	}
}

void nico::Texture::activeTexture()
{
	glActiveTexture(GL_TEXTURE0 + texture);
}

void nico::Texture::bind()
{
	glBindTexture(GL_TEXTURE_2D, id);
}

void nico::Texture::setType(aiTextureType type)
{
	this->type = type;
}

const char* nico::Texture::getPath() const
{
	return path;
}

const int nico::Texture::getWidth() const
{
	return width;
}

const int nico::Texture::getHeight() const
{
	return height;
}

uint32_t nico::generate3dNoiseTexture(uint32_t x, uint32_t y, uint32_t z, uint32_t numberOfPoints = 10)
{
	const size_t SIZE = (size_t)x * (size_t)y * (size_t)z;
	//create the texture data
	unsigned char* data = new unsigned char[SIZE];

	//create an array of points
	std::list<glm::vec3> points(numberOfPoints);

	//fill the array of points
	for (auto it = points.begin(); it != points.end(); it++) {
		(*it).x = rand() % x;
		(*it).y = rand() % y;
		(*it).z = rand() % z;
	}

	float biggestValue = 0;
	glm::vec3 p;

	for (size_t i = 0; i < SIZE; i++) {
		float distance = 1e9;
		p.x = (i + 1) % x;
		p.y = ((i / x) + 1) % y;
		p.z = (i) / (y * x);

		for (auto it = points.begin(); it != points.end(); it++) {
			float dst = glm::distance(p, *it);
			if (dst < distance) {
				distance = dst;
			}
		}
		if (biggestValue < distance)
			biggestValue = distance;
		data[i] = distance;	
	}

	//range the values between 0 and 255
	for (size_t i = 0; i < SIZE; i++) {
		data[i] = 255 - (data[i] / biggestValue * 255);
	}

	uint32_t tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_3D, tex);

	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, x, y, z, 0, GL_RED, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_3D);

	delete[] data;

	glBindTexture(GL_TEXTURE_3D, NULL);
	return tex;
}