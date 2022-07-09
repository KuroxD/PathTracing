#include "BVH_Node.h"
#include <algorithm>
#include <iostream>

BVH_Node::BVH_Node()
	:m_box(AABB::InValid)
{
	left = nullptr;
	right = nullptr;
	miss = -1;
	index = 0;
}

BVH_Node::BVH_Node(std::vector<Hitable*>& hitables)
	:BVH_Node()
{
	if (hitables.size() == 0)
		return;

	int index = 0;
	Build(hitables.begin(), hitables.end(), index);

	miss = -1;
	BuildThread();
}

BVH_Node::BVH_Node(const std::vector<Hitable*>::iterator begin, const std::vector<Hitable*>::iterator end,int& curIndex)
{
	Build(begin, end, curIndex);
}

void BVH_Node::PackData(std::vector<float>& indexBuffer, std::vector<float>& dataBuffer) const
{
	std::cout << "BVH_Node:" <<index<< "  miss:" << miss;
	std::cout << "   left:" << left << "->" << (left == nullptr ? -1 : left->index) << "  |  right:" << right << "->" << (right == nullptr ? -1 : right->index) << std::endl;

	/*std::cout << "  maxPoint:" << GetBoundingBox().m_maxPoint.x << " " << GetBoundingBox().m_maxPoint.y << " " << GetBoundingBox().m_maxPoint.z << std::endl;
	std::cout << "  minPoint:" << GetBoundingBox().m_minPoint.x << " " << GetBoundingBox().m_minPoint.y << " " << GetBoundingBox().m_minPoint.z << std::endl;*/
	//std::cout << std::endl;
	//vec2(dataPos,miss)
	indexBuffer.push_back(dataBuffer.size() / 4);
	indexBuffer.push_back(miss);

	//BVH_Node:(vec4(minPoint,0),vec4(maxPoint,0))
	const AABB& aabb = GetBoundingBox();
	for (int i = 0; i < 3; i++) {
		dataBuffer.push_back(aabb.m_minPoint[i]);
	}
	dataBuffer.push_back(0);
	for (int i = 0; i < 3; i++) {
		dataBuffer.push_back(aabb.m_maxPoint[i]);
	}
	dataBuffer.push_back(0);

	if (left != nullptr) {
		left->PackData(indexBuffer, dataBuffer);
	}

	if (right != nullptr)
		right->PackData(indexBuffer, dataBuffer);
		
}

void BVH_Node::Build(const std::vector<Hitable*>::iterator begin, const std::vector<Hitable*>::iterator end,int& curIndex)
{
	index = curIndex++;
	size_t size = end - begin;
	if (size == 0) {
		return;
	}
	if (size == 1) {
		left = *begin;
		left->index = curIndex++;
		right = nullptr;
		m_box = left->GetBoundingBox();
		return;
	}

	if (size == 2) {
		left = *begin;
		left->index = curIndex++;
		right = *(begin + 1);
		right->index = curIndex++;
		m_box = left->GetBoundingBox() + right->GetBoundingBox();
		return;
	}

	int axis = rand() % 3;
	std::sort(begin, end, [&](Hitable* r, Hitable* l)->bool {return r->GetBoundingBox().m_minPoint[axis] < l->GetBoundingBox().m_minPoint[axis]; });
	auto leftPart = std::vector<Hitable*>(begin, begin + size / 2);
	auto rightPart = std::vector<Hitable*>(begin + size / 2, end);
	assert(size == leftPart.size() + rightPart.size());

	left = new BVH_Node(leftPart.begin(), leftPart.end(), curIndex);
	right = new BVH_Node(rightPart.begin(), rightPart.end(), curIndex);
	m_box = left->GetBoundingBox() + right->GetBoundingBox();
}

void BVH_Node::BuildThread()
{
	if (left != nullptr) {
		if (right != nullptr) {
			left->miss = right->index;
			right->miss = miss;
			left->BuildThread();
			right->BuildThread();
		}
		else {	//leaf
			left->miss = miss;
			left->BuildThread();
		}
	}
}



HitRecord BVH_Node::RayIn(const Ray& ray) const
{
	bool isLeftBoxHit = (left != nullptr) ? left->GetBoundingBox().Hit(ray) : false;
	bool isRightBoxHit = (right != nullptr) ? right->GetBoundingBox().Hit(ray) : false;

	HitRecord hitRec;
	if (isLeftBoxHit) {
		if (isRightBoxHit) {	
			HitRecord leftRec = left->RayIn(ray);
			HitRecord rightRec = right->RayIn(ray);

			if (leftRec.IsHit()) {
				if (rightRec.IsHit() && rightRec.m_hitT < leftRec.m_hitT)
					hitRec = rightRec;
				else
					hitRec = leftRec;
			}
			else 
				hitRec = rightRec;
		}
		else {
			hitRec = left->RayIn(ray);
		}
	}
	else if (isRightBoxHit) {
		hitRec = right->RayIn(ray);
	}
	
	return hitRec;
}
