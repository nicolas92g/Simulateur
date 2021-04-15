#include "TextInput.h"

#define SURFACE_WRITABLE .95


nico::TextInput::TextInputObj* nico::TextInput::textObject;

nico::TextInput::TextInput(Window* win, glm::vec2 position, glm::vec2 scale) : Button(win, position, scale)
{
	constructor();
}

nico::TextInput::TextInput(Window* win, Texture* tex, glm::vec2 position, glm::vec2 scale) : Button(win, tex, position, scale)
{
	constructor();
}

nico::TextInput::~TextInput()
{
	delete escape;
	delete del;
	delete back;
	delete left;
	delete right;
}

void nico::TextInput::draw(Shader* shader)
{
	preDrawInstructions(shader);

	//set the position of the cursor chen clicked and select button
	if (isClicked()) {
		selected = true;

		float mouse = win->getCursorX() - textPosition.x;
		float advance = 0;
		for (size_t i = 0; i < text.size(); i++)
		{
			if (mouse < advance) {
				this->cursorIndex = i;
				break;
			}
			advance += textObject->Text.getAdvance(text[i], scale.y);
		}
	}
	//unselect button
	if (!isHover() and win->Mouse(GLFW_MOUSE_BUTTON_LEFT)) {
		selected = false;
	}
	Object2d::draw(shader);
	glm::vec2 initialPos = glm::vec2(position.x - scale.x * SURFACE_WRITABLE * .5, position.y);

	if (selected) {
		
		if (right->isDown()) {
			right1();
		}
		if (left->isDown()) {
			left1();
		}
		if(back->isDown()){
			if (cursorIndex > -1) {
				std::string::const_iterator it = text.begin() + cursorIndex;
				text.erase(it);
				left1();
			}	
		}
		char C = win->getChar();
		if (!(limited and ((double)calculateAdvance(0, lastLetterIndex) + (double)textObject->Text.getAdvance(C, scale.y) >= scale.x * SURFACE_WRITABLE)) and C) {
			bool authorization = false;
			if (lettersAuthorized != "") {
				for (size_t i = 0; i < lettersAuthorized.length(); i++)
				{
					if (C == lettersAuthorized[i]) {
						authorization = true;
						break;
					}
				}
			}
			else{ authorization = true; }

			if (authorization) {
				std::string::const_iterator it = text.begin() + (size_t(cursorIndex) + 1);
				text.insert(it, C);
				right1();
			}
		}
		

		//update keys
		back->getState();
		left->getState();
		right->getState();
		del->getState();

		

		//this update the indices of the text to render
		updateTextToRender();
		
		//cursor
		if (cursorIndex < 0)
			textObject->cursor.setPosition(initialPos);
		else if (!cursorIndex)
			textObject->cursor.setPosition(initialPos + glm::vec2(calculateAdvance(firstLetterIndex, firstLetterIndex), .0f));
		else
			textObject->cursor.setPosition(initialPos + glm::vec2(calculateAdvance(firstLetterIndex, cursorIndex), .0f));

		textObject->cursor.setTransparency(transparency);
		textObject->cursor.setMultiplyColor(glm::vec3(1) - mixedColor);
		textObject->cursor.setScale(glm::vec2(2, scale.y));

		if (time > 500.0) {
			cursorDisplay = !cursorDisplay;
			time = 0;
		}
		else {
			time += (glfwGetTime() / 1000.0);
		}
			
		if(cursorDisplay)
			textObject->cursor.draw(shader);
	}
	else {
		glActiveTexture(GL_TEXTURE0);
		HoverTexture->bind();

		object->draw(shader);
	}
	//text draw :
	textObject->Text.updateDisplay(win);
	textObject->Text.print(text.substr(firstLetterIndex, size_t(lastLetterIndex) + 1), initialPos.x, initialPos.y - scale.y * .25, scale.y * .8, glm::vec4(0,0,0, this->transparency), false);
}

void nico::TextInput::setText(std::string newText)
{
	text = newText;
	cursorIndex = 0;
}

void nico::TextInput::setFilterLetters(std::string chars)
{
	//check that we do not put many times the same letters
	std::string checkedChars = "";
	for (size_t i = 0; i < chars.length(); i++)
	{
		bool finded = false;
		for (size_t j = 0; j < checkedChars.length(); j++) {
			if (checkedChars[j] == chars[i]) {
				finded = true;
				break;
			}
		}
		if (!finded)
			checkedChars += chars[i];
	}
	lettersAuthorized = checkedChars;
}

std::string nico::TextInput::getText() const
{
	return text;
}

std::string nico::TextInput::getFilterLetters() const
{
	return this->lettersAuthorized;
}

void nico::TextInput::constructor()
{
	if (textObject == nullptr) {
		textObject = new TextInputObj();
	}
	selected = false;
	limited = true;

	escape =new Key(win, GLFW_KEY_ESCAPE);
	back =	new Key(win, GLFW_KEY_BACKSPACE);
	del =	new Key(win, GLFW_KEY_DELETE);
	left =	new Key(win, GLFW_KEY_LEFT);
	right = new Key(win, GLFW_KEY_RIGHT);

	cursorIndex = -1;
	cursorDisplay = true;
	firstLetterIndex = 0;
	lastLetterIndex = -1;
	lettersAuthorized = "";
	text = "";
}

void nico::TextInput::left1()
{
	if (cursorIndex <= -1)
		return;
	
	if (cursorIndex == firstLetterIndex and firstLetterIndex > 0) {
		firstLetterIndex--;
	}
	cursorIndex--;
}

void nico::TextInput::right1()
{
	if (cursorIndex >= (int)text.size() - 1)
		return;

	if (cursorIndex > lastLetterIndex)
		firstLetterIndex++;

	cursorIndex++;
}


float nico::TextInput::calculateAdvance(int beginI, int lastI)
{
	float advance = .0f;

	for (int i = beginI; (i <= lastI) and (i < int(text.size())); i++)
	{
		advance += textObject->Text.getAdvance(text[i], scale.y * .8f);
	}
	return advance;
}

void nico::TextInput::updateTextToRender()
{
	while ((calculateAdvance(firstLetterIndex, lastLetterIndex) < scale.x * SURFACE_WRITABLE) and (lastLetterIndex < text.size())) {
		lastLetterIndex++;
	}

	if (lastLetterIndex >= text.size())
		lastLetterIndex = text.size() - 1;

	while ((calculateAdvance(firstLetterIndex, lastLetterIndex) > scale.x * SURFACE_WRITABLE) and lastLetterIndex > firstLetterIndex) {
		lastLetterIndex--;
	}
}
