#pragma once
#include "EdgeManager.hpp"
#include <fstream>
constexpr int GL_ArraySize = 30;

class File_IO
{
private:
	
	static const int max = GL_ArraySize;
	int vertex_count = 0;

public:
	int adj_matrix[max][max] = { 0 };
	int matrixBaseIndex = 0;

	void create_adj_matrix(VertexManager &V, EdgeManager &E);
	std::string matrixToString(int current_vertices, bool startFromZero);
	void writeInFile(int current_vertices);
	void readFromFile(bool startFromZero);
	void constructGraph(VertexManager &V, EdgeManager &E);
	void saveCurrentProfile(int *currentProfile, int size);
	void loadLastProfile(int *currentProfile, int size);
};

