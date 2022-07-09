#pragma once
#include <glm/glm.hpp>
#include "Ray.hpp"

class AABB {
public:
	AABB();
	AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint);

	bool Hit(const Ray& ray) const;
	bool Hit(const Ray& ray, float& tMin, float& tMax) const;

	const AABB operator+(const AABB& rh) const;
	AABB& operator+=(const AABB& rh);
	inline bool IsValid() const { return m_minPoint.x <= m_maxPoint.x&& m_minPoint.y <= m_maxPoint.y&& m_minPoint.z <= m_maxPoint.z; }

	glm::vec3 m_minPoint;
	glm::vec3 m_maxPoint;

	const static AABB InValid;
};