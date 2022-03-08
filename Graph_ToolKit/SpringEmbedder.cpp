#include "SpringEmbedder.hpp"

void SpringEmbedder::getWorkingForces(int(&adjMatrix)[maxSize][maxSize], VertexManager & v, EdgeManager & e)
{
	eachCell temp, forceForI;
	int count = 1;
	system("cls");

	xAcc.resize(40);
	yAcc.resize(40);


	for (auto i = v.vertices.begin(); i != v.vertices.end(); i++) {
		float forceX = 0, forceY = 0;

		for (auto j = std::next(i,1) ; j != v.vertices.end(); j++) {

			//euclidean distance between i and j
			float dx = j->shape.getPosition().x - i->shape.getPosition().x;
			float dy = j->shape.getPosition().y - i->shape.getPosition().y;
			float dist = (dx*dx + dy*dy);
			dist = sqrt(dist);


			float xArrtAcc = k * dist * dx;
			float yArrtAcc = k * dist * dy;
			float xRepAcc = k * dx / pow(dist, 2);
			float yRepAcc = k * dy / pow(dist, 2);

			if (adjMatrix[i->numbering][j->numbering] == 1) {
				xAcc[i->numbering] += xArrtAcc;
				xAcc[j->numbering] -= xArrtAcc;
				yAcc[i->numbering] += yArrtAcc;
				yAcc[j->numbering] -= yArrtAcc;
			}
			else {
				xAcc[i->numbering] -= xRepAcc;
				xAcc[j->numbering] += xRepAcc;
				yAcc[i->numbering] -= yRepAcc;
				yAcc[j->numbering] += yRepAcc;
			}

			//std::cout << xAcc[i->numbering] << " " << xAcc[j->numbering] << " " << yAcc[i->numbering] << " " << yAcc[j->numbering] << "\n";
		}
	}

	applyForce(v, e);
}

void SpringEmbedder::applyForce(VertexManager & v, EdgeManager & e)
{
	int vertex_size = 40;
	for (auto i = v.vertices.begin(); i != v.vertices.end(); i++) {
		float fposX = (i->shape.getPosition().x + (xAcc[i->numbering]));
		float fposY = (i->shape.getPosition().y + (yAcc[i->numbering]));
		//std::cout << fposX << " " << fposY << "\n";

		if (fposX - vertex_size / 2 > 0 && fposX + vertex_size / 2 < 1080 &&
			fposY - vertex_size / 2 > 0 && fposY + vertex_size / 2 < 640) {

			i->shape.setPosition(fposX, fposY);
			i->name.setPosition(i->shape.getPosition().x - 5, i->shape.getPosition().y - 5);
		}
	}

	//xAcc.clear();
	//yAcc.clear();

}
