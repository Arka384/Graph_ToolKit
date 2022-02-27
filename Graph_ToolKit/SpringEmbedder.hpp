#pragma once
#include "VertexManager.hpp"
#include "EdgeManager.hpp"

typedef struct eachCell {
	float forceX, forceY;
}eachCell;

class SpringEmbedder
{
	static const int maxSize = 40;
public:
	std::list<eachCell> totalForce, forceForNodes;
	std::vector<float> xAcc, yAcc;
	float idealSpringLength = 200.f;
	float k = 0.001;
	//float cRepulsive = 1, cAttractive = 2;

	void getWorkingForces(int(&adjMatrix)[maxSize][maxSize], VertexManager &v, EdgeManager &e);
	void applyForce(VertexManager &v, EdgeManager &e);
};

