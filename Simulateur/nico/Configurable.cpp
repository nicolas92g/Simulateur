#include "Configurable.h" 
#ifdef NICO_RENDERING_DEBUG

std::unordered_map<std::string, nico::Configurable*> nico::Configurable::objects;

void nico::Configurable::setName(std::string name)
{
	if (objects.find(name) != objects.end()) {
		std::cout << "nico::Configurable Error: try to name an object with an already assigned name : \"" + name + "\" !\n";
		return;
	}
	this->name = name;
	objects[name] = this;
}

nico::Configurable* nico::Configurable::object(std::string name)
{
	 auto it = objects.find(name);
	 if (it == objects.end())
		 return nullptr;
	 return it->second;
}

void nico::Configurable::printAll()
{
	for (auto it = objects.begin(); it != objects.end(); it++)
	{
		std::cout << it->first << " = " << it->second << "\n";
	}
}
#endif // NICO_RENDERING_DEBUG