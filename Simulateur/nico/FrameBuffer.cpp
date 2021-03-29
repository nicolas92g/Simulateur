#include "FrameBuffer.h"

nico::FrameBuffer::FrameBuffer() {
	glGenFramebuffers(1, &this->id);
	glBindFramebuffer(GL_FRAMEBUFFER, id);
}

void nico::FrameBuffer::bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->id);
}

void nico::FrameBuffer::unbind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

bool nico::FrameBuffer::checkFrameBufferComplete()
{
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	return (glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
}
