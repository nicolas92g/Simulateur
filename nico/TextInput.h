#pragma once
#include <nico/Button.h>
#include <nico/textRenderer.h>

namespace nico{
	/**
	 * @brief allow the user to input a string 
	 */
	class TextInput : public Button{
	public:
		/**
		 * @brief create the input
		 * \param win
		 * \param position
		 * \param scale
		 */
		TextInput(Window* win, glm::vec2 position = { 10, 10 }, glm::vec2 scale = { 100, 30 });
		/**
		 * @brief create the input
		 * \param win
		 * \param tex
		 * \param position
		 * \param scale
		 */
		TextInput(Window *win, Texture* tex, glm::vec2 position, glm::vec2 scale);
		/**
		 * @brief destroy the input
		 */
		~TextInput();
		/**
		 * @brief draw and update the input
		 * \param shader
		 */
		void draw(Shader* shader);
		/**
		 * @brief select this input
		 */
		void select() { selected = true; }
		/**
		 * @brief unselect this input
		 */
		void close() { selected = false; }
		/**
		 * @brief allow to know if this object is selected
		 * \return 
		 */
		bool isSelected() const { return selected; }
		/**
		 * @brief change the string in the input
		 * \param newText
		 */
		void setText(std::string newText);
		/**
		 * @brief set all the chars allowed in the input
		 * \param chars
		 */
		void setFilterLetters(std::string chars);
		/**
		 * @brief return the inputed string 
		 * \return 
		 */
		std::string getText() const;
		/**
		 * @brief get all the chars that are alowed in the input
		 * \return 
		 */
		std::string getFilterLetters() const;
		/**
		 * @brief convert this object in the inputed string
		 * \return
		 */
		operator std::string() { return getText(); };
		
	protected:
		void constructor();
		void left1();
		void right1();
		float calculateAdvance(int beginI, int lastI);
		void updateTextToRender();

		int cursorIndex;
		int firstLetterIndex;
		int lastLetterIndex;
		std::string text;
		glm::vec2 textPosition;
		std::string lettersAuthorized;

		bool selected;
		bool cursorDisplay;
		bool limited;

		Key* del;
		Key* escape;
		Key* back;
		Key* left;
		Key* right;

		double time;

		struct TextInputObj {
			nico::TextRenderer Text;
			nico::Object2d cursor;
			nico::Texture CursorTex;
			TextInputObj() : CursorTex(NICO_TEXTURES_PATH"textCursor.png"), cursor(&CursorTex) {}
		};

		static TextInputObj* textObject;

	private:
		using Button::operator bool;
	};
	
	
}
