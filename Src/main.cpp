#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include "test.h"

#include <vector>
#include <chrono>
#include <random>

#include "opengl/Shader.h"
#include "opengl/Texture.h"
#include "opengl/RenderPass.h"

std::uniform_real_distribution<float> fMap(0.0f, 1.0f);
std::default_random_engine engine;

const int WIDTH = 784;
const int HEIGHT = 784;

int main(void)
{
    GLFWwindow* window;
    
    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(WIDTH, HEIGHT, "RayTracing", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glewInit();
    Scence* cornellBoxScence = Scence::CreateCornellbox();
    //TestRayTracing(cornellBoxScence);

    
    Shader rayTraceShader("Src/Shader/common.vs", "Src/Shader/rayTracing.fs");
    Shader middleShader("Src/Shader/common.vs", "Src/Shader/middle.fs");
    Shader finalShader("Src/Shader/common.vs", "Src/Shader/tex2Screen.fs");

    std::vector<float> indexBuffer;
    std::vector<float> dataBuffer;

    cornellBoxScence->bvhRoot->PackData(indexBuffer, dataBuffer);

    /*std::cout << std::endl;
    for (int i = 0; i < indexBuffer.size() / 2; i++) {
        std::cout <<i<<":"<< indexBuffer[2 * i] << " " << indexBuffer[2 * i + 1] << std::endl;
    }

    std::cout << std::endl;
    for (int i = 0; i < dataBuffer.size() / 4; i++) {
        std::cout << i << ":" << dataBuffer[4 * i] << " " << dataBuffer[4 * i + 1] << " " << dataBuffer[4 * i + 2] << " " << dataBuffer[4 * i + 3] << std::endl;
    }*/
    std::cout << "IndexSize:" << indexBuffer.size() / 2 <<" dataSize:"<<dataBuffer.size()/4<< std::endl;

    Texture2D dataTex(dataBuffer.size() / 4, 1, GL_RGBA32F, GL_RGBA, GL_FLOAT, dataBuffer.data());
    Texture2D indexTex(indexBuffer.size()/2, 1, GL_RG32F, GL_RG, GL_FLOAT, indexBuffer.data());
    Texture2D lastFrameTex(WIDTH, HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);
    Texture2D curFrameTex(WIDTH, HEIGHT, GL_RGBA32F, GL_RGBA, GL_FLOAT, 0);

    RenderPass rayTracePass(WIDTH, HEIGHT,rayTraceShader,curFrameTex);
    RenderPass middlePass(WIDTH, HEIGHT, middleShader, lastFrameTex);
    RenderPass finalPass(WIDTH, HEIGHT, finalShader);
    

    


    /* Loop until the user closes the window */
    int frameCount = 0;
    int lastFrame = 0;
    auto last_time = std::chrono::system_clock::now();
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        //raytracing pass
        rayTracePass.program.Use();
        glUniform1i(glGetUniformLocation(rayTracePass.program.id, "frameCount"), frameCount);
        rayTracePass.program.SetTexture(indexTex, "indexTex", 0);
        rayTracePass.program.SetTexture(dataTex, "dataTex", 1);
        rayTracePass.program.SetTexture(lastFrameTex, "lastFrameTex", 2);
        rayTracePass.Draw();

        //pass 2
        middlePass.program.Use();
        middlePass.program.SetTexture(curFrameTex, "curFrameTex", 3);
        middlePass.Draw();

        //pass 3
        finalPass.program.Use();
        finalPass.program.SetTexture(curFrameTex, "curFrameTex", 0);
        finalPass.Draw();
        
        /* Swap front and back buffers */
        GLCall(glfwSwapBuffers(window));

        /* Poll for and process events */
        GLCall(glfwPollEvents());
        frameCount++;

        auto curr_time = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - last_time);
        if (duration.count() > 1000) {
            std::cout << "FPS:" << (frameCount-lastFrame) / (double)duration.count() * 1000 << std::endl;
            last_time = curr_time;
            lastFrame = frameCount;
        }
    }

    glfwTerminate();
    return 0;
}