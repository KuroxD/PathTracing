#pragma once
#include <GL/glew.h>
#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <string>

#include "VAO.h"
#include "Shader.h"
using namespace glm;

class RenderPass {
public:
    unsigned int fbo = 0;
    VAO vao;
    std::vector<unsigned int> colorAttachments;
    Shader program;
    int width,height;

    RenderPass(int width, int height, const Shader& program, Texture2D outTex=Texture2D(), VAO vao = VAO());

    void Draw(); 
};
