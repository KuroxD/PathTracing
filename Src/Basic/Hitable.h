#pragma once
#include "Ray.hpp"
#include "AABB.h"
#include <glm/glm.hpp>
#include <vector>

struct Vertex {
	glm::vec3 pos;
	glm::vec3 normal;
	glm::vec2 uv;

	// vec4(pos.xyz,uv.x) + vec4(normal.xyz,uv.y) 
	void AppendData(std::vector<float>& data) {
		for (int i = 0; i < 3; i++) {
			data.push_back(pos[i]);
		}
		data.push_back(uv.x);
		for (int i = 0; i < 3; i++) {
			data.push_back(normal[i]);
		}
		data.push_back(uv.y);
	}
};

struct HitRecord {
	HitRecord() :m_ray(nullptr), m_hitT(-1.0f) {};
	HitRecord(const Vertex& hitPoint, const Ray* ray=nullptr, float hitT=-1.0f,bool hit=false)
	:hit(hit),hitPoint(hitPoint),m_ray(ray),m_hitT(hitT) {
		
	}
	inline bool IsHit() const { return hit; }

	bool hit = false;
	const Ray* m_ray;
	Vertex hitPoint;
	float m_hitT;
};

class Hitable {
public:
	Hitable() :miss(-1),index(0) {};

	virtual HitRecord RayIn(const Ray& ray) const = 0;
	virtual const AABB GetBoundingBox() const = 0;
	virtual void PackData(std::vector<float>& indexBuffer, std::vector<float>& dataBuffer) const = 0;
	
	//For Thread
	int miss;
	int index;
	virtual void BuildThread()=0;
};