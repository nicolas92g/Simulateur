#pragma once
#include <string>
#include <iostream>
#include "openGlObject.h"
#include "nicoFunctions.h"

namespace nico {
	class GlObject
	{
	protected:
		uint32_t id;
	public:
		GlObject(unsigned int id_);
		GlObject();

		virtual void bind() = 0;

		uint32_t getId() const;
	};
}
