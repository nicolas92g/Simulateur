#include "NumberInput.h"
#define REP_BEFORE_TAKING_MOUSE 10

nico::NumberInput::NumberInput(Window* win, glm::vec2 position, glm::vec2 scale):
	TextInput(win, position, scale)
{
	this->lettersAuthorized = "1234567890.-";// "+*/^";
	this->timeStayedClick = 0;
	this->step = 0.01;
	this->text = "0";

	maxValue = DBL_MAX;
	minValue = DBL_MIN;
}

void nico::NumberInput::operator=(const double newValue)
{
	setValue(newValue);
}

bool nico::NumberInput::operator<(const double value)
{
	return getValue() < value;
}

bool nico::NumberInput::operator>(const double value)
{
	return getValue() > value;
}

const double nico::NumberInput::getValue() const
{
	return std::max(minValue, std::min(maxValue, getDisplayedValue()));
}

const double nico::NumberInput::getStep() const
{
	return this->step;
}

void nico::NumberInput::setValue(double value)
{
	value = std::max(minValue, std::min(maxValue, value));

	text = std::to_string(value);

	uint32_t i = text.size() - 1;
	while (i > 0) {
		if (text[i - 1] == '.')
			return;

		if (text[i] == '0')
			text.erase(i);

		i--;
	}
}

void nico::NumberInput::setStep(double step)
{
	if (step)
		this->step = step;
}

void nico::NumberInput::setLimits(double min, double max)
{
	if (min >= max) return;

	minValue = min;
	maxValue = max;
}

const double nico::NumberInput::getMin() const
{
	return minValue;
}

const double nico::NumberInput::getMax() const
{
	return maxValue;
}

void nico::NumberInput::draw(Shader* shader)
{
	if (!this->win->Mouse(GLFW_MOUSE_BUTTON_LEFT)) {
		this->timeStayedClick = 0;
	}
	else {
		this->timeStayedClick++;
	}
	const glm::vec2 pos = { this->position.x , this->win->getHeight() - this->position.y };

	if (selected) {
		if (this->timeStayedClick == REP_BEFORE_TAKING_MOUSE) {
			this->win->setCursorPos(pos.x, pos.y);
		}
		else if (this->timeStayedClick > REP_BEFORE_TAKING_MOUSE) {
			this->setValue(this->getDisplayedValue() + (win->getCursorX() - pos.x) * this->step);
			this->win->setCursorPos(pos.x, pos.y);
			this->win->hideCursor(true);
		}
		else {
			this->win->hideCursor(false);
		}
		this->setValue(this->getDisplayedValue());
	}

	TextInput::draw(shader);
} 

void nico::NumberInput::update()
{
	if (!this->text.length())
		this->text = "0";
}

double nico::NumberInput::getDisplayedValue() const
{
	if (!this->text.length())
		return minValue;

	try {
		return stod(this->text);
	}
	catch (...) {
		return minValue;
	}
}
