#include "AABB.h"
#include <algorithm>

const AABB AABB::InValid(glm::vec3(1), glm::vec3(-1));

AABB::AABB()
{
    *this = InValid;
}

AABB::AABB(const glm::vec3& minPoint, const glm::vec3& maxPoint)
    :m_minPoint(minPoint),m_maxPoint(maxPoint)
{
}

bool AABB::Hit(const Ray& ray) const
{
    float tMin=0, tMax=ray.GetTMax();
    return Hit(ray, tMin, tMax);
}

bool AABB::Hit(const Ray& ray, float& tMin, float& tMax) const
{
    const glm::vec3 origin = ray.GetOrigin();
    const glm::vec3 dir = ray.GetDir();

    for (size_t i = 0; i < 3; i++) {
        float div = 1.0 / dir[i];
        float t0 = (m_minPoint[i] - origin[i]) * div;
        float t1 = (m_maxPoint[i] - origin[i]) * div;

        if (div<0.0f)
            std::swap(t0, t1);

        tMin = std::fmax(tMin, t0);
        tMax = std::fmin(tMax, t1);
        if (tMax < tMin)
            return false;
    }
    return true;
}

const AABB AABB::operator+(const AABB& rh) const
{
    glm::vec3 minPoint = glm::min(m_minPoint, rh.m_minPoint);
    glm::vec3 maxPoint = glm::max(m_maxPoint, rh.m_maxPoint);
    return AABB(minPoint, maxPoint);
}

AABB& AABB::operator+=(const AABB& rh)
{
    m_minPoint = glm::min(m_minPoint, rh.m_minPoint);
    m_maxPoint = glm::max(m_maxPoint, rh.m_maxPoint);
    return *this;
}
