#pragma once
#include <nico/window.h>
#include <nico/NumberInput.h>
#include <config.hpp>
#include <json/json.h>
#include <unordered_map>
#include <nico/Configurable.h>

#ifdef NICO_RENDERING_DEBUG
namespace nico {
	/**
	 * @class Editor
	 * @brief not finished
	 */
	class Editor : public Object2d {
	public:
		Editor(Window* win);

		void draw(Shader* shader);

		void setRelativeWidth(float relativeToScreenSize);

	protected:
		Button background;
		Window* win;

		float relativeWidth;//this stock the relative width wanted
		float realRelativeWidth;//this is the actual relative width that is smoothly changed to relativeWidth
		void updateRelativeWidth();

	public:
		static float* getVecFromJson(Json::Value root);
		


	protected:
		template<typename T>
		struct Property {
			std::string name;
			std::string objectName;
			T func;
			Property(std::string objectName, std::string propertyName, T func) {
				this->objectName = objectName;
				this->name = propertyName;
				this->func = func;
			}
		};

		std::list<Property<void(nico::Configurable::*)(double)>> doubles;
		
	};

}
#else
	
namespace nico {
	/**
	 * @class Editor
	 * @brief not finished
	 */
	struct Editor :public Object2d { Editor(Window*) : Object2d(nullptr){} void draw(Shader*) {} };
}
#endif


