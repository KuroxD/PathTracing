#pragma once
#include "../Basic/Triangle.h"
#include <glm/glm.hpp>
#include <vector>
#include <iostream>

class Model
{
public:
	glm::mat4 modelMat;

	std::vector<Triangle> fBuffer;
	std::vector<Vertex>* vertexBuffer;
	std::vector<Vertex>::const_iterator begin;
	std::vector<Vertex>::const_iterator end;

	inline const glm::vec3& GetPos(int fIndex, int vIndex) const { return fBuffer[fIndex].GetVertex(vIndex).pos; }
	inline const glm::vec3& GetNormal(int fIndex, int vIndex) const { return fBuffer[fIndex].GetVertex(vIndex).normal; }
	inline const glm::vec2& GetUV(int fIndex, int vIndex) const { return fBuffer[fIndex].GetVertex(vIndex).uv; }
	void SetColor(glm::vec3 color) { 
		for (auto& tri : fBuffer) {
			tri.SetColor(color);
		}
	}
	void SetEmission(glm::vec3 e) {
		for (auto& tri : fBuffer) {
			tri.SetEmission(e);
		}
	}

	void ClearModel();
	Model(std::vector<Vertex>* vertexBuffer,const char* filePath);
	~Model();
	void LoadModel(const char* filePath);
};
