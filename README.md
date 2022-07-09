# PathTracing
基于OpenGL的Path Tracing

# Intro
  找工作用demo，opengl下的path tracing，目前就做了Cornell box的场景 
  
# Catalog
- Assetes:Cornellbox模型
- Dependencies:相关库文件，主要是GLFW,GLEW,GLM
- Src:源代码文件
	- Basic:光线求交相关，包括线索BVH构建，AABB、三角形求交
	- opengl:opengl相关接口封装，包括模型读取，VAO，纹理，shader，渲染pass封装
	- Shader:glsl shader源代码，主要是rayTracing.fs处理path tracing核心逻辑
	- main.cpp:程序入口
	- test.cpp:CPU端path Tracing测试debug用

# Preview
![image](Assets/preview/cornell%20box.png)
