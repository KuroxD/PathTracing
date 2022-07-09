#pragma once
#include "Hitable.h"
#include <glm/glm.hpp>
#include <vector>


class Triangle :public Hitable {
public:
	Triangle(std::vector<Vertex>* vertexBuffer,int v0,int v1,int v2);

	HitRecord RayIn(const Ray& ray) const;
	const AABB GetBoundingBox() const;
	glm::vec3 GetNormal() const;
	inline Vertex& GetVertex(int index) const { return (*vertexBuffer)[vertexIndex[index]]; }

	Vertex BarycentricInterpolate(glm::vec3 baryCoord) const;
	void PackData(std::vector<float>& indexBuffer, std::vector<float>& dataBuffer) const;
	void BuildThread();
	std::vector<Vertex>* vertexBuffer;
	int vertexIndex[3];

	//Material
	void SetColor(const glm::vec3& color) { kd = color; }
	void SetEmission(const glm::vec3& e) { emission = e; }
	glm::vec3 kd;
	glm::vec3 emission;
};