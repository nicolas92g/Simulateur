#pragma once
#include <nico/openGlObject.h>
#include <glad/glad.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postProcess.h>
#include <assimp/pbrmaterial.h>
#include <list>



namespace nico {
	struct texParameters {
		/*
		4 types of texture wraping :
		GL_REPEAT: The default behavior for textures. Repeats the texture image.
		GL_MIRRORED_REPEAT: Same as GL_REPEAT but mirrors the image with each repeat.
		GL_CLAMP_TO_EDGE: Clamps the coordinates between 0 and 1. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
		GL_CLAMP_TO_BORDER: Coordinates outside the range are now given a user-specified border color with glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		*/
		GLint wrap_s;
		/*
		4 types of texture wraping :
		GL_REPEAT: The default behavior for textures. Repeats the texture image.
		GL_MIRRORED_REPEAT: Same as GL_REPEAT but mirrors the image with each repeat.
		GL_CLAMP_TO_EDGE: Clamps the coordinates between 0 and 1. The result is that higher coordinates become clamped to the edge, resulting in a stretched edge pattern.
		GL_CLAMP_TO_BORDER: Coordinates outside the range are now given a user-specified border color with glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
		*/
		GLint wrap_t;
		/*
		4 types of texture filtering :
		GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel size and uses nearest neighbor interpolation for texture sampling.
		GL_LINEAR_MIPMAP_NEAREST: takes the nearest mipmap level and samples that level using linear interpolation.
		GL_NEAREST_MIPMAP_LINEAR: linearly interpolates between the two mipmaps that most closely match the size of a pixel and samples the interpolated level via nearest neighbor interpolation.
		GL_LINEAR_MIPMAP_LINEAR: linearly interpolates between the two closest mipmaps and samples the interpolated level via linear interpolation.
		*/
		GLint min_filter;
		/*
		4 types of texture filtering :
		GL_NEAREST_MIPMAP_NEAREST: takes the nearest mipmap to match the pixel size and uses nearest neighbor interpolation for texture sampling.
		GL_LINEAR_MIPMAP_NEAREST: takes the nearest mipmap level and samples that level using linear interpolation.
		GL_NEAREST_MIPMAP_LINEAR: linearly interpolates between the two mipmaps that most closely match the size of a pixel and samples the interpolated level via nearest neighbor interpolation.
		GL_LINEAR_MIPMAP_LINEAR: linearly interpolates between the two closest mipmaps and samples the interpolated level via linear interpolation.
		*/
		GLint mag_filter;

		texParameters() {
			wrap_s = GL_REPEAT;
			wrap_t = GL_REPEAT;
			min_filter = GL_LINEAR_MIPMAP_LINEAR;
			mag_filter = GL_LINEAR;
		}
		texParameters(int wrap_s, int wrap_t, int min_filter, int mag_filter) {
			this->wrap_s = wrap_s;
			this->wrap_t = wrap_t;
			this->min_filter = min_filter;
			this->mag_filter = mag_filter;
		}
	};
	/**
	 * @brief this is a TEXTURE_2D that you can load easily with an image path and then use to draw on a mesh or anything
	 */
	class Texture : public GlObject
	{	
	public:
		//this texture constructor assign some default value : RGBA channels, GL_REPEAT wrap
		/**
		 * 
		 * 
		 * \param imageFilePath
		 * \param type
		 */
		Texture(const char* imageFilePath, texParameters param = texParameters(), aiTextureType type = aiTextureType::aiTextureType_BASE_COLOR);
		void useTexture(unsigned int channel);
		void activeTexture();
		void bind();
		aiTextureType type;
		void setType(aiTextureType type);

		const char* getPath() const;
		const int getWidth() const;
		const int getHeight() const;

	private:
		void loadTexture(const char* image, texParameters param = texParameters());
		int width;
		int height;
		int nbrChannels;
		const char* path;


		unsigned int texture;
	};
	
	uint32_t generate3dNoiseTexture(uint32_t xRes, uint32_t yRes, uint32_t zRes, uint32_t numberOfPoints);
}
