#pragma once
#include "Hitable.h"
#include <vector>

/*
	Type:
		BVH_Node:dataPos>=0
		Triangle:-1

	BVH:(type="BVH_Node:dataPos",miss),(type="Triangle",dataPos)}
	DATA: 
		BVH_Node:(vec4(minPoint,0),vec4(maxPoint,0))
		Triangle:(vec4(pos,u),vec4(normal,v))
*/


class BVH_Node :public Hitable {
public:
	BVH_Node();

	BVH_Node(std::vector<Hitable*>& hitables);
	BVH_Node(const std::vector<Hitable*>::iterator begin, const std::vector<Hitable*>::iterator end,int& curIndex);
	
	HitRecord RayIn(const Ray& ray) const;
	const AABB GetBoundingBox() const { return m_box; }

	void PackData(std::vector<float>& indexBuffer, std::vector<float>& dataBuffer) const;
	void Build(const std::vector<Hitable*>::iterator begin, const std::vector<Hitable*>::iterator end,int& curIndex);
	void BuildThread();
	Hitable* left;
	Hitable* right;

	AABB m_box;
};

