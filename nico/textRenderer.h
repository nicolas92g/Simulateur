#pragma once
#include <glm/glm.hpp>
#include <map>
#include <nico/Shader.h>
#include <ft2build.h>
#include <iostream>
#include <glad/glad.h>
#include <glm/gtx/transform.hpp>
#include <nico/vertexBuffer.h>
#include <nico/window.h>
#include "config.hpp"

namespace nico {

	struct Character {
		unsigned int textureID;
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		unsigned int Advance;
	};
	
	/**
	 * @brief this renderer allow to draw text on a window 
	 */
	class TextRenderer
	{
	public:
		/**
		 * create a 2D text renderer with the font path given in parameters
		 * \param font
		 */
		TextRenderer(const char* font = NICO_TEXTURES_PATH"arial.ttf");
		/**
		 * allow to know the window dimension to render well relatively to the window size
		 * \param win
		 */
		void updateDisplay(nico::Window* win);
		/**
		 * allow to know the last text rendering width, this value is changed by : calculateTextRenderingSize();
		 * \return width of the last text rendering
		 */
		unsigned int getLastWidth() const;
		/**
		 * allow to know the last text rendering height, this value is changed by : calculateTextRenderingSize();
		 * \return height of the last text rendering
		 */
		unsigned int getLastHeight() const;
		/**
		 * print function by default with origin at the bottom left corner of the window, you can change position of the rendering, scale
		 * color, and you can disable background color that allow to see the text better
		 * \param text
		 * \param x
		 * \param y
		 * \param scale
		 * \param color
		 * \param background
		 */
		void print(const std::string& text, float x = 5, float y = 5, float scale = 20.0f, glm::vec4 color = glm::vec4(1), bool background = true);
		/**
		 * same as the print function but with the origin at the top left corner of the windows
		 * \param text
		 * \param x
		 * \param y
		 * \param scale
		 * \param color
		 * \param background
		 */
		void printLeftTop(const std::string& text, float x = 5, float y = 5, float scale = 20.0f, glm::vec4 color = glm::vec4(1), bool background = true);
		/**
		 * same as the print function but with the origin at the top right corner of the windows
		 * \param text
		 * \param x
		 * \param y
		 * \param scale
		 * \param color
		 * \param background
		 */
		void printRightTop(const std::string& text, float x = 5, float y = 5, float scale = 20.0f, glm::vec4 color = glm::vec4(1), bool background = true);
		/**
		 * same as the print function but with the origin at the bottom left corner of the windows
		 * \param text
		 * \param x
		 * \param y
		 * \param scale
		 * \param color
		 * \param background
		 */
		void printLeftBottom(const std::string& text, float x = 5, float y = 5, float scale = 20.0f, glm::vec4 color = glm::vec4(1), bool background = true);
		/**
		 * change the background color with an rgba color type 
		 * \param color
		 */
		void setBackgroundColor(glm::vec4 color){ shader.set("backColor", color); }
		/**
		 * change the offset size of the background ( margin arround the background ) 
		 * \param offset
		 */
		void setBackgroundOffset(unsigned int offset) { this->offset = offset; }
		/**
		 * allow to know before rendering the size of it to ajust it position
		 * you can get the size of the rendering with the returned vector or with the getLastHeight() and getLastWidth() calls
		 * call this before a print call with the same args
		 * \param text
		 * \param scale
		 * \return 
		 */
		glm::vec2 calculateTextRenderingSize(std::string text, float scale);
		/**
		 * return the advance in pixel of one character in the current font loaded and with the scale you wan't apply to it
		 * \param asciiCHaracter
		 * \param scale
		 * \return 
		 */
		float getAdvance(char asciiCHaracter, float scale);

		/**
		 * you can use this to add text to render anywhere in your code by changing  ex : TextRenderer::textToPrint += "Hello world";
		 * but you have to print this var yourself this is not done automatically
		 */
		static std::string textToPrint;
	private:

		std::map<char, Character> Characters;
		//shader that render text in 2D
		Shader shader;
		glm::mat4 projectionMatrix;
		unsigned int VAO;
		unsigned int VBO;
		//textRendering position using width and height of the textures letter
		float width;
		float height;
		//background :
		unsigned int offset;

		//scale stuffs
		unsigned int maxHeight;
		float scaleByPixel;

		//size of the windows
		unsigned int windowsWidth;
		unsigned int windowsHeight;

		//calculate width and height of the futur text rendering
		void fillWidthAndHeight(const std::string* text, float scale);
		void renderText(const std::string& text, float x = 5, float y = 5, float scale = 1.0f, glm::vec4 color = glm::vec4(1), bool background = true);
	};
}

