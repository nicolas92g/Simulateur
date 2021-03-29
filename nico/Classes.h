#pragma once
#include <iostream>
#include "Shader.h"
#include <GLFW/glfw3.h>
#include <nico/window.h>
#include <list>


namespace nico {
	/**
	 * @brief this class is abstract, don't create object of it, use it as a mother class  to represent an object that can be drawed
	 * and you have to create : draw(Shader*) and update(); in the new class
	 */
	class DrawableObject 
	{
	public:
		DrawableObject(){}

		/**
		 * generic draw call.
		 * \param Shader pointer
		 */
		virtual void draw(Shader*) = 0;
		/**
		 * generic update call by default do not do anything
		 */
		virtual void update() {}
	};
	/**
	 * @brief this allow to use many keys for one event and to change at any time the keys that are checked
	 */
	class KeysEvent 
	{
	public:
		KeysEvent(Window* windowWhereKeyIsCkecked) {
			this->win = windowWhereKeyIsCkecked;
		}
		KeysEvent(Window* windowWhereKeyIsCkecked, int firstKey) {
			this->win = windowWhereKeyIsCkecked;
			keys.push_back(firstKey);
		}
		/**
		 * @brief add a new key with GLFW_KEY_NAME macros
		 * \param newKey
		 */
		void addKey(int newKey) {
			for (std::list<int>::iterator it = keys.begin(); it != keys.end(); it++)
			{
				if ((*it) == newKey)
					return;
			}
			keys.push_back(newKey);
		}
		/**
		 * @brief check if one of the keys is pressed
		 * \return 
		 */
		const bool press(){
			for (std::list<int>::iterator it = keys.begin(); it != keys.end(); it++)
			{
				if (win->Key(*it))
					return true;
			}
			return false;
		}
		/**
		 * @brief checked if all keys are pressed
		 * \return 
		 */
		const bool allPress() {
			for (std::list<int>::iterator it = keys.begin(); it != keys.end(); it++)
			{
				if (!win->Key(*it))
					return false;
			}
			return true;
		}
		/**
		 * @brief remove the keys
		 */
		void clear() {
			keys.clear();
		}
		/**
		 * @brief remove all the keys and add a new key
		 * \param newKey
		 */
		void clear(int newKey) {
			keys.clear();
			keys.push_back(newKey);
		}
		/**
		 * @brief convert this object in a boolean that is tru if one of the key is pressed
		 * \return 
		 */
		operator bool() {
			return press();
		}
		
	protected:
		std::list<int> keys;
		Window* win;
	};
	/**
	 * @brief (not finished )on/off switch with a keyboard key (first time key is pressed this object become true, and second time this become false , etc..)
	 */
	class KeySwitch : protected KeysEvent
	{
	public:
		KeySwitch(Window* windowWhereKeyIsCkecked, int firstKey = -1, bool defaultValue = false) :
			KeysEvent(windowWhereKeyIsCkecked, firstKey)
		{
			value = defaultValue;
			previousState = value;
			pressed = false;
		}
		/**
		 * @brief return true when this object is changing of value
		 * \return 
		 */
		bool justSwitch() {
			if (press())
				return !pressed;
			return pressed;
		}
		/**
		 * @brief this return the state of the switch on/off --> true/false
		 * \return 
		 */
		operator bool(){
			if (press())
				if(!pressed)
					value = !value;

			pressed = press();

			return value; 
		}
		using KeysEvent::addKey;
		using KeysEvent::clear;
		using KeysEvent::press;

	protected:
		bool value;
		bool pressed;
		bool previousState;
	};
	/**
	 * @brief allow to have multiple information about one key that you can change
	 */
	class Key {
	public:
		/**
		 * @brief create the object
		 * \param win
		 * \param GLFW_KEY
		 */
		Key(Window* win, int GLFW_KEY) {
			this->win = win;
			key = GLFW_KEY;
			previousState = win->Key(key);
			time = 0;
		}
		/**
		 * @brief change the key to read
		 * \param GLFW_KEY
		 */
		void assignKey(int GLFW_KEY) {
			key = GLFW_KEY;
		}
		/**
		 * @brief return if the key is pressed
		 * \return 
		 */
		operator bool(){
			return win->Key(key);
		}
		/**
		 * @brief test if the key was released and is pressing
		 * \return 
		 */
		const bool isDown() {
			return (!previousState and win->Key(key));
		}
		/**
		 * @brief test if the key was a pressed and is releasing
		 * \return
		 */
		const bool isUp() {
			return (previousState and !win->Key(key));
		}
		/**
		 * @brief read the key you have to call this for isDown and isUp
		 */
		void getState() {
			previousState = win->Key(key);
		}
	protected:
		int key;
		Window* win;
		bool previousState;
		double time;
	};
}