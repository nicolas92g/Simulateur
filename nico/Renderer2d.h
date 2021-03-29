#pragma once

#include "window.h"
#include "Texture.h"
#include "Shader.h"
#include "vertexBuffer.h"
#include "config.hpp"
#include "Classes.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <list>
#include "Object2d.h"
#include "Button.h"

namespace nico{
	/**
	 * @brief allow to render some 2D stuff
	 */
	class Renderer2d
	{
	public:
		/**
		 * @brief create the renderer
		 * \param win
		 */
		Renderer2d(Window *win);
		/**
		 * @brief destroy the renderer
		 */
		~Renderer2d();
		/**
		 * @brief add an Object2d to draw in each frame
		 * \param object
		 */
		void addElement(Object2d* object);
		/**
		 * @brief remove an Object2d from the objects to draw
		 * \param object
		 */
		void removeElement(Object2d* object);
		/**
		 * @brief allow to know the size of the window
		 * \param win
		 */
		void assignWindow(Window* win);
		/**
		 * @brief draw all the Objects
		 */
		void frame();
		/**
		 * @brief update the drawing priorities of the Objects
		 */
		void updateLayers();
		/**
		 * @brief return the shader that is used to render 
		 * \return 
		 */
		Shader* Shader() { return &shader; }
		/**
		 * @brief return the window that is assigned to this renderer
		 * \return 
		 */
		Window* Window() { return win; }


	protected:
		nico::Shader shader;
		nico::Window* win;
		std::list<Object2d*> objects;
	};

	
}

