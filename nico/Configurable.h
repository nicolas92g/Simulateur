#pragma once
#include <unordered_map>
#include <config.hpp>
#include <iostream>

namespace nico{
	/**
	 * @brief not finished
	 */
	class Configurable {
	public:
#		ifdef NICO_RENDERING_DEBUG
		void setName(std::string name);

		virtual void importFromJson(std::string jsonString) = 0;

		static Configurable* object(std::string name);
		static void printAll();

	protected:
		std::string name;

	private:
		static std::unordered_map<std::string, Configurable*> objects;
#		endif
	};
}
