#include "openGlObject.h"

nico::GlObject::GlObject(unsigned int id_) : id(id_)
{
}

nico::GlObject::GlObject()
{
	id = 0;
}

void nico::GlObject::bind()
{
}

uint32_t nico::GlObject::getId() const
{
	return id;
}
