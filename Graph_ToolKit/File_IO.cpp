#include "File_IO.hpp"

void File_IO::create_adj_matrix(VertexManager &V, EdgeManager &E)
{
	if (E.edges.size() == 0)
		return;

	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++)
			adj_matrix[i][j] = 0;
	}

	for (auto i = E.edges.begin(); i != E.edges.end(); i++) {
		if (E.directedGraph)
			adj_matrix[i->v1][i->v2] = i->weight;
		else {
			adj_matrix[i->v1][i->v2] = i->weight;
			adj_matrix[i->v2][i->v1] = i->weight;
		}
	}

	matrixBaseIndex = 1;
	if (V.startFromZero)
		matrixBaseIndex = 0;

}

std::string File_IO::matrixToString(int current_vertices, bool startFromZero, bool usingAlpha)
{
	std::string matrixString;
	/*
	if (usingAlpha) {
		//writing alphabets
		for (int i = matrixBaseIndex; i <= current_vertices; i++) {
			std::stringstream s;
			s << (char)(i + 65);
			matrixString.append(s.str());
		}
		matrixString.append("\n");
	}*/

	for (int i = matrixBaseIndex; i <= current_vertices; i++)
	{
		for (int j = matrixBaseIndex; j <= current_vertices; j++) {
			std::stringstream ss;
			ss << adj_matrix[i][j];
			matrixString.append(ss.str());
			matrixString.append(" ");
		}
		matrixString.append("\n");
	}

	std::stringstream ss;
	if(startFromZero) ss << current_vertices + 1;
	else ss << current_vertices;
	matrixString.append("\nNumber of Vertices: ");
	matrixString.append(ss.str());

	return matrixString;
}

void File_IO::reset() {
	for (int i = 0; i < max; i++) {
		for (int j = 0; j < max; j++)
			adj_matrix[i][j] = 0;
	}
}

void File_IO::writeInFile(int current_vertices)
{
	std::ofstream output("output.txt");
	for (int i = matrixBaseIndex; i <= current_vertices; i++)
	{
		for (int j = matrixBaseIndex; j <= current_vertices; j++)
			output << adj_matrix[i][j] << " ";
		output << "\n";
	}
	output.close();
}

void File_IO::readFromFile(bool startFromZero)
{
	std::ifstream input("input.txt");
	int number = 0, i = 1, j = 1, prev_j = 1;
	input >> vertex_count;

	if (startFromZero) {
		i = j = prev_j = 0;
		vertex_count--;
	}

	while (input >> number)
	{
		adj_matrix[i][j] = number;
		if (j == vertex_count) {
			j = prev_j;	//startFromZero -> 0 else 1
			i++;
		}
		else j++;
	}
	input.close();
}

void File_IO::constructGraph(VertexManager &V, EdgeManager &E)
{
	matrixBaseIndex = 1;
	if (V.startFromZero)
		matrixBaseIndex = 0;

	for (int i = matrixBaseIndex; i <= vertex_count; i++)
		V.createVertex();

	for (int i = matrixBaseIndex; i <= vertex_count; i++) {
		for (int j = matrixBaseIndex; j <= vertex_count; j++) {
			if (adj_matrix[i][j] != 0) {

				std::string si = std::to_string(i);
				char const *csi = si.c_str();
				std::string sj = std::to_string(j);
				char const *csj = sj.c_str();
				std::string sw = std::to_string(adj_matrix[i][j]);
				char const *csw = sw.c_str();

				E.createEdge(V.vertices, (char*)csi, (char*)csj, (char*)csw, false);
			}
		}
	}

}

void File_IO::saveCurrentProfile(int *currentProfile, int size)
{
	std::ofstream profile("profile/settings.txt");
	for (int i = 0; i < size; i++) {
		profile << currentProfile[i] << "\n";
	}
}

void File_IO::loadLastProfile(int * currentProfile, int size)
{
	std::ifstream profile("profile/settings.txt");
	for (int i = 0; i < size; i++) {
		profile >> currentProfile[i];
	}
}
