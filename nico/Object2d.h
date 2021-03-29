#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include "Shader.h"
#include "Classes.h"
#include "Texture.h"
#include <nico/Configurable.h>

namespace nico {
	class Plane;
	/**
	 * @class Object2d
	 * @brief this define a 2d texture that you can draw with a position, a scale and rotation
	 * the 0, 0 position is on the bottom left corner of the window and the texture position point is on the middle
	 * this Object can't resize up a texture so the scale of the object is limited by the texture resolution
	 * the scale is in pixel
	 * the position is in pixel
	 * the rotation is in degrees
	 * you can set a layer which allow to say which one will be draw on top of which one
	 */
	class Object2d
	{
	public:
		/**
		 * @brief create the object with a default Texture, position, scale and rotation
		 * \param image
		 * \param position
		 * \param scale
		 * \param rotAngle
		 * \param rotAxe
		 */
		Object2d(Texture* image, glm::vec2 position = glm::vec2(0), glm::vec2 scale = glm::vec2(1), float rotAngle = .0f, glm::vec3 rotAxe = glm::vec3(0, 0, 1));
		/**
		 * @brief this change the texture to be draw
		 * \param image
		 */
		void assignTexture(Texture* image);
		/**
		 * @brief this draw the texture
		 * \param shader
		 */
		virtual void draw(Shader* shader);

		void importFromJson(std::string jsonString) {}
		/**
		 * @brief return if the layer of the other object is greater
		 * \param other
		 * \return 
		 */
		bool operator<(Object2d other);

		//modifiers
		/**
		 * @brief change location relatively to the bottom left corner
		 * \param pos
		 */
		void setPosition(glm::vec2 pos);
		/**
		 * @brief set the scale of the texture in pixel
		 * \param scale
		 */
		void setScale(glm::vec2 scale);
		/**
		 * @brief set the scale relatively to the texture size
		 * \param relativeScale
		 */
		void setRelativeScale(glm::vec2 relativeScale);
		/**
		 * @brief set the rotation of the Texture 
		 * you can change the axe of rotation to rotate it in a 3d way
		 * \param angle
		 * \param axe
		 */
		void setRotation(float angle, glm::vec3 axe = { 0,0,1 });
		/**
		 * @brief this value is in ]0, 1]
		 * where 0 represent a full translucent color
		 * and 1 is opaque
		 * \param multiplyToAlpha
		 */
		void setTransparency(float multiplyToAlpha);
		/**
		 * @brief apply a color filter on your texture
		 * (the color will be multiply with the color texture)
		 * \param color
		 */
		void setMultiplyColor(glm::vec3 color);
		/**
		 * set the drawing priority (the object with the biggest layer will be draw first)
		 * work with a nico::Renderer2d instance else is useless
		 * \param layer
		 */
		void setLayer(unsigned int layer);

		//accessors
		glm::vec2 getPosition() const;
		glm::vec2 getScale() const;
		float getRotationAngle() const;
		glm::vec3 getRotationAxe() const;
		float getTransparency() const;
		glm::vec3 getMixedColor() const;
		unsigned int getLayer() const;

		static void CreatePlane();
		static void DestroyPlane();

	protected:
		void preDrawInstructions(Shader* shader);

		glm::vec2 position;
		glm::vec2 scale;
		glm::vec3 rotationAxe;
		float rotationAngle;

		Texture* texture;

		float transparency;
		glm::vec3 mixedColor;

		unsigned int layer;

		static Plane* object;
	};

	class Plane {
	public:
		Plane();
		void draw(Shader* shader);

	protected:
		uint32_t VAO;
	};
}
