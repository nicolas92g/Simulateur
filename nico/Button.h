#pragma once
#include <nico/Classes.h>
#include <nico/Texture.h>
#include <nico/Object2d.h>
#include "config.hpp"

namespace nico {
	/**
	 * @brief a basic button that change of texture when mouse is hover the button and when button is clicked
	 * the button have some default textures that are ugly so change them
	 */
	class Button : public Object2d
	{
	public:
		/**
		 * @brief need a window to locate the cursor 
		 * need a texture to draw on the button 
		 * \param win
		 * \param buttonTexture
		 * \param position
		 * \param scale
		 */
		Button(Window* win, Texture* buttonTexture, glm::vec2 position = glm::vec2(10), glm::vec2 scale = { 100, 40 });
		/**
		 * @brief this use the defaults textures
		 * \param win
		 * \param position
		 * \param scale
		 */
		Button(Window* win, glm::vec2 position = glm::vec2(10), glm::vec2 scale = { 100, 40 });
		/**
		 * @brief set the texture to be draw when the cursor is on the button but not cliked
		 * \param hover
		 */
		void assignHoverTexture(Texture* hover);
		/**
		 * @brief set the texture to draw when the button is clicked
		 * \param clicked
		 */
		void assignClickedTexture(Texture* clicked);
		/**
		 * @brief return if the mouse is on the button
		 * \return 
		 */
		bool isHover();
		/**
		 * @brief return if the button is clicked
		 * \return 
		 */
		bool isClicked();
		/**
		 * @brief convert the button into a boolean that is true when the button is clicked
		 */
		operator bool() { return isClicked(); };
		/**
		 * @brief draw the button 
		 * \param shader
		 */
		void draw(Shader* shader);

		/**
		 * @brief this remove the default textures from the memory 
		 */
		static void destroyDefaultTextures() { delete defaultTextures[0]; delete defaultTextures[1]; delete defaultTextures[2]; }
	protected:
		//window where is renderer the button
		Window* win;

		//different Textures of the button
		Texture* HoverTexture;
		Texture* Clickedtexture;

		static Texture* defaultTextures[3];
		

	private:
		//disable Rotations
		using Object2d::setRotation;
		using Object2d::getRotationAngle;
		using Object2d::getRotationAxe;
	};
}


