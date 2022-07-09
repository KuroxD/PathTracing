#pragma once
#include <string>
#include <GL/glew.h>
#include "Define.h"
#include "Texture.h"


class Shader {
public:
	Shader(const std::string& vertexPath, const std::string& fragmentPath);
	unsigned int CreateProgram(const std::string& vertexSource, const std::string& fragmentSource);
	unsigned int CompileShader(unsigned int type,const std::string& source);


	void SetTexture(Texture2D tex, const char* varName,unsigned int slot);

	void Use() const { GLCall(glUseProgram(id)); }
	unsigned int id;
};