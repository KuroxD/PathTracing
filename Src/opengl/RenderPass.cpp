#include "RenderPass.h"

RenderPass::RenderPass(int width, int height, const Shader& program, Texture2D outTex, VAO vao)
	:vao(vao),program(program), width(width), height(height)
{
	if (outTex.id != 0) {
		glGenFramebuffers(1, &fbo);
		glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		glBindTexture(GL_TEXTURE_2D, outTex.id);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outTex.id, 0);
		GLenum buffers[] = { GL_COLOR_ATTACHMENT0 };
		glDrawBuffers(1, buffers);
		ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
	}
		
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderPass::Draw() {
	program.Use();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, fbo));
	vao.Use();
	glClear(GL_COLOR_BUFFER_BIT);
	
	vao.Draw();
	glBindVertexArray(0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glUseProgram(0);
}
