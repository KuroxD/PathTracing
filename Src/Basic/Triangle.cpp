#include "Triangle.h"
#include <iostream>
using namespace glm;

Triangle::Triangle(std::vector<Vertex>* vertexBuffer, int v0, int v1, int v2)
	:vertexBuffer(vertexBuffer),kd(vec3(0)),emission(vec3(0))
{
	vertexIndex[0] = v0;
	vertexIndex[1] = v1;
	vertexIndex[2] = v2;
}

HitRecord Triangle::RayIn(const Ray& ray) const
{
	vec3 e0 = GetVertex(1).pos - GetVertex(0).pos;
	vec3 e1 = GetVertex(2).pos - GetVertex(0).pos;
	vec3 pv = cross(ray.GetDir(), e1);
	float det = dot(e0, pv);
	vec3 tv = ray.GetOrigin() - GetVertex(0).pos;
	vec3 qv = cross(tv, e0);

	vec4 uvt;
	uvt.x = dot(tv, pv);
	uvt.y = dot(ray.GetDir(), qv);
	uvt.z = dot(e1, qv);
	for (int i = 0; i < 3; i++)
		uvt[i] /= det;
	uvt.w = 1.0 - uvt.x - uvt.y;
	if (all(greaterThanEqual(uvt, vec4(0.0))) && (uvt.z < ray.GetTMax())) {
		return HitRecord(BarycentricInterpolate(vec3(uvt.w, uvt.x, uvt.y)), &ray, float(uvt.z),true);
	}
	return HitRecord();
}

const AABB Triangle::GetBoundingBox() const
{
	glm::vec3 minPoint = glm::min(GetVertex(0).pos, GetVertex(1).pos);
	minPoint = glm::min(minPoint, GetVertex(2).pos);

	glm::vec3 maxPoint = glm::max(GetVertex(0).pos, GetVertex(1).pos);
	maxPoint = glm::max(maxPoint, GetVertex(2).pos);

	return AABB(minPoint, maxPoint);
}

glm::vec3 Triangle::GetNormal() const
{
	const Vertex& v0 = GetVertex(0);
	const Vertex& v1 = GetVertex(1);
	const Vertex& v2 = GetVertex(2);
	vec3 e0 = normalize(v1.pos - v0.pos);
	vec3 e1 = normalize(v2.pos - v0.pos);
	return normalize(cross(e0, e1));
}

Vertex Triangle::BarycentricInterpolate(glm::vec3 baryCoord) const
{
	Vertex res;
	const Vertex& v0 = GetVertex(0);
	const Vertex& v1 = GetVertex(1);
	const Vertex& v2 = GetVertex(2);

	res.pos = baryCoord.x * v0.pos + baryCoord.y * v1.pos + baryCoord.z * v2.pos;
	res.uv = baryCoord.x * v0.uv + baryCoord.y * v1.uv + baryCoord.z * v2.uv;
	res.normal= baryCoord.x * v0.normal + baryCoord.y * v1.normal + baryCoord.z * v2.normal;

	return res;
}

void Triangle::PackData(std::vector<float>& indexBuffer, std::vector<float>& dataBuffer) const
{
	std::cout << "Triangle:" << index << "  miss:" << miss << std::endl;


	//(type = "Triangle", dataPos)
	indexBuffer.push_back(dataBuffer.size() / -4.0);
	indexBuffer.push_back(miss);

	//Triangle:(vec4(pos,u),vec4(normal,v))
	for (int i = 0; i < 3; i++) {
		const Vertex& v = GetVertex(i);
		for (int j = 0; j < 3; j++) {
			dataBuffer.push_back(v.pos[j]);
		}
		dataBuffer.push_back(v.uv[0]);

		for (int j = 0; j < 3; j++) {
			dataBuffer.push_back(v.normal[j]);
		}
		dataBuffer.push_back(v.uv[1]);
	}

	for (int i = 0; i < 3; i++) {
		dataBuffer.push_back(kd[i]);
	}
	dataBuffer.push_back(0);
	for (int i = 0; i < 3; i++) {
		dataBuffer.push_back(emission[i]);
	}
	dataBuffer.push_back(0);
}

void Triangle::BuildThread()
{
	return;
}
