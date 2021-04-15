#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <nico/Classes.h>
#include "Shader.h"

typedef glm::vec3 vec3;

namespace nico {
	/**
	 * @class Object3d
	 * @brief contain a object that is able to be drawed
	 * and contain all vector characteristic of a 3D object
	 */
	class Object3d
	{

	public:
		/**
		 * this constructor assign an drawable object, the type of the object have be a children of nico::DrawObject 
		 * and must have an draw(Shader*) call and can have an draw() call that use an internal shader pointer to draw
		 * it can also have a update function that modify what you need before drawing
		 * \param objectTodraw
		 * \param position
		 * \param scale
		 * \param rotationAxe
		 * \param angle
		 */
		Object3d(DrawableObject* objectTodraw , vec3 position = vec3(0.0f), vec3 scale = vec3(1.0f), vec3 rotationAxe = vec3(1,0,0), float angle = 0.0f);
		/**
		 * this function update the drawing of the object using vars
		 * call it before drawing but after modifying the public members vars
		 */
		void update();
		/**
		 * draw the object in the 3D world \n
		 * if you modify any of the public members vars, the object won't update his display
		 * call this->update() to update drawing of the object
		 * \param shader to use to draw the object
		 */
		virtual void draw(Shader *shader);
		/**
		 * this define the object to draw .
		 * 
		 * \param obj
		 */
		void assignDrawableObject(DrawableObject* obj);
		/**
		 * you don't need to update if you call this function.
		 * 
		 * \param position
		 * \param scale
		 * \param rotationAxe
		 * \param angle
		 */
		void modifyValues(vec3 position, vec3 scale, vec3 rotationAxe , float angle);

		//modifiers
		void setPos(vec3 pos);
		/**
		 * block assigment of (0,0,0) to scale 
		 * you have to use update() to apply modifications
		 * \param scale
		 */
		void setScale(vec3 scale);
		/**
		 * modify rotation
		 * you have to use update() to apply modifications
		 * \param rotAxe
		 * \param angleInRadians
		 */
		void setRotation(vec3 rotAxe, float angleInRadians);
		/**
		 * block assigment of (0,0,0) to vector axe 
		 * you have to use update() to apply modifications
		 * \param rotAxe
		 */
		void setRotationAxe(vec3 rotAxe);
		/**
		 * modify rotation angle 
		 * you have to use update() to apply modifications
		 * \param angle
		 */
		void setRotationAngle(float angle);

		//accessors
		/**
		 * \return Position of the object
		 */
		vec3 getPos() const;
		/**
		 * \return Scale of the object
		 */
		vec3 getScale() const;
		/**
		 * \return rotation axe of the object
		 */
		vec3 getRotationAxe() const;
		/**
		 * \return rotation angle of the object
		 */
		float getRotationAngle() const;
		/**
		 * @brief return the world transformation of the Object3d
		 */
		glm::mat4 getWorldTransformation() const;

	protected:
		//model matrix to send to shader in each draw call
		glm::mat4 model;
		//object to draw and to update
		DrawableObject* object;
		//vars use to gen model matrix
		vec3 position;
		vec3 scale;
		vec3 rotationAxe;
		float rotationAngle;
	};

}


