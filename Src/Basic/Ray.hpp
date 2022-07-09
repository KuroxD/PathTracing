#pragma once
#include <glm/glm.hpp>
#include <float.h>


class Ray {
public:
	Ray(const glm::vec3& origin = glm::vec3(0), const glm::vec3& dir = glm::vec3(1)):m_origin(origin),m_dir(dir) {
		m_tMax = FLT_MAX;
	}

	inline glm::vec3 operator()(float t) const { return m_origin + t * m_dir; }
	inline glm::vec3 At(float t) const { return m_origin + t * m_dir; }

	inline glm::vec3 GetOrigin() const { return m_origin; }
	inline glm::vec3 GetDir() const { return m_dir; }
	inline glm::vec3 GetColor() const { return m_color; }
	inline float GetTMax() const { return m_tMax; }
	void SetTMax(float tMax) { m_tMax = tMax; }

	inline void Update(const glm::vec3& origin, const glm::vec3& dir, const glm::vec3& color) {
		m_origin = origin;
		m_dir = dir;
		m_color = color;
	}

private:
	glm::vec3 m_origin;
	glm::vec3 m_dir;
	glm::vec3 m_color;
	float m_tMax;
};