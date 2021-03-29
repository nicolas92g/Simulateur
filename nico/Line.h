#pragma once
#include <nico.hpp>
#include <glm/glm.hpp>
namespace nico {
	/**
	 * @brief this allow to draw a vector in 3D
	 */
	class Line
	{
	public:
		/**
		 * @brief create the object with a initial location, vector and size
		 * \param position
		 * \param vector
		 * \param size
		 */
		Line(glm::vec3 position = glm::vec3(0.0f), glm::vec3 vector = glm::vec3(0.0f,1.0f,0.0f), float size = 1.0f);
		/**
		 * @brief draw the line
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief change the position of the vector
		 * \param position_
		 */
		void setPosition(glm::vec3 position_);
		/**
		 * @brief set the vector to draw
		 * \param vector_
		 */
		void setVector(glm::vec3 vector_);
		/**
		 * @brief set the size of the line
		 * \param size_
		 */
		void setSize(float size_);
	protected:
		glm::vec3 position;
		glm::vec3 vector;
		float size;

		nico::VertexArray VAO;
	};
}
