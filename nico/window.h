#pragma once
#include <glm/common.hpp>
#include <glm/glm.hpp>
#include <glad/glad.h>
//#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <deque>
#include <stb_image.h>
#include "openGlObject.h"
#include "glDebugging.h"


#define DEFAULT_SCREEN_SIZE glm::vec2(600,400)
namespace nico {
	/**
	 * @brief this create a open-gl window
	 */
	class Window
	{
	private:
		GLFWwindow* glfwWin;
		int fps;
		double deltaTime;
		double time;
		bool pauseIfUnfocused;

	public:
		Window(int openglMajorVersion,
			int openglMinorVersion , 
			bool debugMode, 
			int antialiasingLevel, 
			const char* title = "window", 
			glm::vec2 widthAndHeight = DEFAULT_SCREEN_SIZE);
		/**
		 * @brief destroy the window
		 */
		~Window();
		/**
		 * @brief return if the key specified is pressed
		 * \param GLFW_KEY
		 * \return 
		 */
		bool Key(int GLFW_KEY);
		/**
		 * @brief return if the mouse button specified is pressed
		 * \param GLFW_MOUSE
		 * \return 
		 */
		bool Mouse(int GLFW_MOUSE);
		/**
		 * @brief return if something is trying to close this window
		 * \return 
		 */
		bool shouldClose() const;
		/**
		 * @brief window will do something special depending of OS
		 */
		void requestAttention() const;
		/**
		 * @brief change the texture of the icon using the path of the image
		 * \param imagePath
		 */
		void setIcon(const char* imagePath);
		/**
		 * @brief to call at the end of each frame
		 */
		void endFrame();
		/**
		 * @brief this will resize the window at the window resolution
		 */
		void maximise();
		/**
		 * @brief this method hide the cursor or unhide it
		 * \param hide
		 */
		void hideCursor(bool hide);
		/**
		 * @brief set the window in fullscreen
		 */
		void setFullscreen(bool mode);

		//accessors :
		/**
		 * @brief return the GLFWwindow object of this window
		 * \return 
		 */
		GLFWwindow* glfw() const;
		/**
		 * @brief return a vector with the size of the window
		 * \return 
		 */
		glm::vec2 getSize() const;
		/**
		 * @brief return the width in pixel of the window
		 * \return 
		 */
		int getWidth() const;
		/**
		 * @brief return the height in pixel of the window
		 * \return 
		 */
		int getHeight() const;
		/**
		 * @brief get the cursor x location on the window
		 * \return 
		 */
		double getCursorX() const;
		/**
		 * @brief get the cursor y location on the window
		 * \return
		 */
		double getCursorY() const;
		/**
		 * @brief get the age if the window in seconds
		 * \return 
		 */
		double getTime() const;
		/**
		 * @brief get the time of the frame in seconds
		 * \return 
		 */
		double getDeltaTime() const;
		/**
		 * @brief get the framerate
		 * \return 
		 */
		int getFps() const;


		//modifiers :
		/**
		 * @brief change size of the screen
		 * \param width
		 * \param height
		 */
		void setSize(int width, int height);
		/**
		 * @brief change the title of the window
		 * \param newTitle
		 */
		void setTitle(const char* newTitle);
		/**
		 * @brief change the pos of the window in the screen
		 * \param x
		 * \param y
		 */
		void setPos(int x, int y);
		/**
		 * @brief change the cursor location
		 * \param x
		 * \param y
		 */
		void setCursorPos(double x, double y);
		/**
		 * @brief request a close or remove a request (true or false)
		 * \param shouldClose
		 */
		void setShouldClose(bool shouldClose);

		//callbacks access //this is not perfect, it's better to use the callback
		char getChar() { return returnThenAssign<char>(&input, 0); }

	protected:
		//static functions :
		static void framebuffer_size_callback(GLFWwindow* glfw, int width, int height);
		static void getChar(GLFWwindow* glfw, unsigned int Char);

		static char input;

	};
}
