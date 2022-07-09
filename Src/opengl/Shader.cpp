#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>


std::string ParseShader(const std::string& filepath) {
	std::ifstream stream(filepath);

	std::string line;
	std::stringstream ss;
	while (getline(stream, line)) {
		ss << line << "\n";
	}
	return ss.str();
}

Shader::Shader(const std::string& vertexPath,const std::string &fragmentPath)
{
	std::string fragmentSource = ParseShader(fragmentPath);
	std::string vertexSource = ParseShader(vertexPath);

	id = CreateProgram(vertexSource, fragmentSource);
}

unsigned int Shader::CreateProgram(const std::string& vertexSource, const std::string& fragmentSource)
{
	unsigned int program;
	GLCall(program = glCreateProgram());
	unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexSource);
	unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

unsigned int Shader::CompileShader(unsigned int type, const std::string& source)
{
	unsigned int id;
	GLCall(id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* msg = new char[length+1];
		glGetShaderInfoLog(id, length, &length, msg);
		std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? " vertex " : " fragment ") << "shader" << std::endl;
		std::cout << msg << std::endl;

		glDeleteShader(id);
		return 0;
	}
	
	return id;
}

void Shader::SetTexture(Texture2D tex, const char* varName,unsigned int slot)
{
	Use();
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, tex.id));
	GLCall(glUniform1i(glGetUniformLocation(id, varName), slot));

}

