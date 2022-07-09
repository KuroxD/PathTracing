#include "Model.h"
#include <fstream>
#include <io.h>
#include <iostream>
#include <sstream>
#include <string>
#include <cassert>

void Model::ClearModel()
{
	if (!fBuffer.empty()) {
		vertexBuffer->erase(begin, end);
		fBuffer.clear();
	}
}

Model::Model(std::vector<Vertex>* vertexBuffer, const char* filepath)
	:vertexBuffer(vertexBuffer),begin(vertexBuffer->end()),end(vertexBuffer->end())
{
	LoadModel(filepath);
}

Model::~Model()
{

}

void Model::LoadModel(const char* filepath) {
	ClearModel();

	std::ifstream in;
	in.open(filepath, std::ifstream::in);
	std::string line;
	assert(in.good());

	std::vector<glm::vec3> posBuffer;
	int cur_mtl = 0;
	while (!in.eof()) {
		std::getline(in, line);
		if (!line.compare(0, 2, "v "))		//vertex
		{
			std::istringstream iss(line.substr(2));
			glm::vec3 v;
			iss >> v.x>>v.y>>v.z;

			posBuffer.push_back(v);
		}
		else if (!line.compare(0, 2, "f "))	//face
		{
			std::istringstream iss(line.substr(2));
			int index[3];
			char trash;
			iss >> index[0]  >> index[1] >>  index[2];

			int vSize = vertexBuffer->size();
			Vertex v[3];
			for (int i = 0; i < 3; i++) {
				v[i].pos = posBuffer[index[i] - 1];
				vertexBuffer->push_back(v[i]);
			}
			
			Triangle t(vertexBuffer, vSize, vSize+1, vSize+2);
			glm::vec3 normal = t.GetNormal();
			for (int i = 0; i < 3; i++) {
				t.GetVertex(i).normal = normal;
			}
			fBuffer.push_back(t);
		}
	}
	in.close();
	end = vertexBuffer->end();
}