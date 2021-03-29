#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include <nico/Classes.h>
#include <nico/Configurable.h>
#include <json/json.h>
#include "Editor.h"

namespace nico {
	/**
	 * @class BasicLight
	 * @brief an abstract class (don't use it)
	 */
	class BasicLight : public Configurable
	{
	public:
		/**
		 * default constructor to assign values
		 */
		BasicLight();
		/**
		 * modify position of your light
		 * \param pos_
		 */
		void setPos(glm::vec3 pos_);
		/**
		 * modify directly your light 
		 * \param ambient
		 * \param diffuse
		 * \param specular
		 */
		void setLightValues(glm::vec3 ambient, glm::vec3 color, glm::vec3 specular);
		/**
		 * easy way to modify your light 
		 * \param lightColor
		 * \param ambientIntensity
		 * \param diffuseIntensity
		 * \param specularIntensity
		 */
		void setLight(glm::vec3 lightColor, 
			float ambientIntensity = 0.1f, float diffuseIntensity = 0.5f, float specularIntensity = 0.5f);
		void setColor(glm::vec3 diff);
		void setSpecular(glm::vec3 specular);
		void setAmbient(glm::vec3 amb);
		/**
		 * \return light position in world coordinates
		 */
		glm::vec3 getPos();
		
		virtual void sendToShader(Shader* shader_) = 0;
	protected:
		glm::vec3 pos;

		glm::vec3 ambient;
		glm::vec3 color;
		glm::vec3 specular;

	};
	/**
	 * class Light
	 * @brief create a point light with a distance attenuation, if your scene is gamma corrected,
	 * only use the linear value to control the attenuation else use the 
	 * setRange method or use setLightAttenuationValues() with all the params
	 */
	class Light : public BasicLight
	{
	public:
		typedef enum class range {
			_7,
			_13,
			_20,
			_32,
			_50,
			_65,
			_100,
			_160,
			_200,
			_325,
			_600,
			_3250,
			_inf
		} range;
	public:
		/**
		 * use this in a scene that is not gamma corrected
		 * fill with range enum that modify the attenuation in fonction of
		 * distance the light have to reach
		 * the distance is not really exact because it change with other values
		 * but use it like a strength indicator
		 * \param range
		 */
		Light(range range = range::_50);
		 /**
		  * in case of a gamma corrected scene only use linear param to choose the attenuation strength
		  * \param linear
		  * \param quadratic
		  * \param constant
		  */
		Light(float linear, float quadratic = 0.0f , float constant = 1.0f);
		/**
		 * use this in a scene that is not gamma corrected
		 * fill with range enum that modify the attenuation in fonction of
		 * distance the light have to reach
		 * \param range
		 */
		void setRange(range range);
		/**
		 * in case of a gamma corrected scene only use linear param to choose the attenuation strength
		 * \param linear
		 * \param quadratic
		 * \param constant
		 */
		void setLightAttenuationValues(float linear, float quadratic = 0.0f , float constant = 1.0f);
		/**
		 * assign to shader the following uniform :\n
		 * struct Light{\n
		 *     vec3 position;\n
		 *     vec3 ambient;\n
		 *     vec3 diffuse;\n
		 *     vec3 specular;\n
		 *     float shininess;\n
		 * \n
		 *     float constant;\n
		 *     float linear;\n
		 *     float quadratic;\n
		 * }\n
		 * uniform Light light[];\n
		 * and fill this uniform \n
		 * uniform uint numberOfLight;
		 * \param shader_
		 */
		virtual void sendToShader(Shader* shader_);
		/**
		 * start the couting of lights to draw,
		 * to call at the begin of the draw loop before sending any light to a shader
		 * this is done automatically by the nico::Renderer
		 */
		static void clear();

		void importFromJson(std::string jsonString);

	protected:
		float constant;
		float linear;
		float quadratic;

	private:
		static uint32_t number;
	};
	/**
	 * @class SpotLight
	 * @brief create a spot in your world, innerAngle is the angle of the spot in degrees\n
	 * outerAngle is the angle of the smooth transition between dark and full light of the spot
	 */
	class SpotLight : public Light
	{
	public:
		/**
		 * this create a spot light directly
		 * angles are limited in [0.1 , 360.0]
		 * \param direction
		 * \param innerAngle
		 * \param outerAngle
		 */
		SpotLight(glm::vec3 direction = glm::vec3(1,0,0), float innerAngle = 17.5f, float outerAngle = 19.0f);
		/**
		 * this directly modify the spot
		 * angles are limited in [0.1 , 360.0]
		 * \param direction
		 * \param innerAngle
		 * \param outerAngle
		 */
		void setSpotValues(glm::vec3 direction, float innerAngle, float outerAngle);
		/**
		 * this modify the direction of the spot light
		 * \param direction
		 */
		void setDirection(glm::vec3 direction);
		/**
		 * this modify the angles of the spot, the inner = full light (angle is in degrees)
		 * outer = smooth transition
		 * \param innerAngle
		 * \param outerAngle
		 */
		void setSpotAngle(float innerAngle, float outerAngle);
		/**
		 * set the angle of the spot light and set the transition size relativly to angle with a percentage (angle is in degrees)
		 * \param angle
		 * \param transistion percentage
		 */
		void setSpotAngle(float angle, int transitionPercentageOfAngle = 10); 
		/**
		 * assign to shader the following uniform :\n
		 * struct Light{\n
		 *     vec3 position;\n
		 *     vec3 ambient;\n
		 *     vec3 diffuse;\n
		 *     vec3 specular;\n
		 *     float shininess;\n
		 * \n
		 *     float constant;\n
		 *     float linear;\n
		 *     float quadratic;\n
		 * \n
		 *     vec3 direction;\n
		 *     float cutOff;\n
		 *     float outerCutOff;\n
		 * }\n
		 * uniform Light light[];\n
		 * and fill this uniform\n
		 * uniform uint numberOfSpot;
		 * \param shader_
		 */
		virtual void sendToShader(Shader* shader);
		/**
		 * start the couting of lights to draw,
		 * to call at the begin of the draw loop before sending any light to a shader
		 * this is done automatically by the nico::Renderer
		 */
		static void clear();

	protected:
		glm::vec3 direction;
		float innerAngle;
		float outerAngle;

		float checkAngle(float angle);

	private:
		static uint32_t number;
	};
	/**
	 * @brief a light that is at an infinite distance, 
	 * generaly use for the sun.
	 * you can only create one directional light in your world
	 * if you use nico::Renderer
	 */
	class DirectionalLight : public BasicLight
	{
	public:
		/**
		 * only need a direction to work
		 * \param direction
		 */
		DirectionalLight(glm::vec3 direction = glm::vec3(0,-1, 0));
		/**
		 * change direction of the directional light 
		 * can't be (0,0,0) and it is normalized
		 * \param direction
		 */
		void setDirection(glm::vec3 direction);
		/**
		 * \return normalized direction of the light 
		 */
		glm::vec3 getDirection() const;
		/**
		 * assign to shader the following uniform :\n
		 * struct DirLight{\n
		 *     vec3 direction;\n
		 *     vec3 ambient;\n
		 *     vec3 diffuse;\n
		 *     vec3 specular;\n
		 *     float shininess;\n
		 * }\n
		 * uniform DirLight dirLight;\n
		 * \param shader_
		 */
		virtual void sendToShader(Shader* shader);

		void importFromJson(std::string jsonString);

	protected:
		glm::vec3 direction;

	//hide innerited position elements witch are useless in this class
	private:
		using BasicLight::pos;
		using BasicLight::getPos;
		using BasicLight::setPos;
		 
	};

	void clearLights();
}

