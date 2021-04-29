#pragma once
#include "TextInput.h"
#include <float.h>

namespace nico{
	/**
	 * @brief this allow to get some floating numbers from the user
	 * input is under form of text input and mouse input
	 * if you stay clicked on the input, moving mouse will change the value
	 */
	class NumberInput : public TextInput{
	public:
		/**
		 * @brief create the input
		 * \param win
		 * \param position
		 * \param scale
		 */
		NumberInput(Window* win, glm::vec2 position = { 10, 10 }, glm::vec2 scale = { 250, 40 });
		/**
		 * @brief get the real value from the input
		 * \return 
		 */
		const double getValue() const;
		/**
		 * @brief return the step that is add on mouse change
		 * \return 
		 */
		const double getStep() const;
		/**
		 * @brief directly change the number in the input
		 * \param value
		 */
		void setValue(double value);
		/**
		 * @brief set the number that is add when you use the cursor to change the number
		 * \param step
		 */
		void setStep(double step);
		/**
		 * @brief set the maximun and minimun value of the numberInput
		 */
		void setLimits(double min, double max);
		/**
		 * @brief return the minimun value
		 */
		const double getMin() const;
		/**
		 * @brief return the maximun value
		 */
		const double getMax() const;
 
		/**
		 * @brief draw and update the input
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief convert this object into the real value of this input
		 * \return 
		 */
		operator double() { return this->getValue(); }

	protected:
		uint32_t timeStayedClick;//count how many times user stay left click on the button
		double step;//step to add with the mouse

		double minValue;
		double maxValue;

		void update();
		double getDisplayedValue() const;

	private:
		using TextInput::setFilterLetters;
		using TextInput::getFilterLetters;
		using TextInput::getText;
		using TextInput::setText;
		using TextInput::operator std::string;
		using Button::operator bool;
	};
}
