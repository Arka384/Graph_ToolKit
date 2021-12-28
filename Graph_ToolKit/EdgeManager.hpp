#pragma once
#include "VertexManager.hpp"
#include "Edge.hpp"
class EdgeManager
{
private:
	sf::Sprite arrow;
	sf::Texture arrow_tex;
	sf::Color edgeColour = sf::Color::White, weightColour = sf::Color::Red;
	float edgeSize = 10.f, weightSize = 30;
	int e1 = 0, e2 = 0;
	int copy_w = 1;
	sf::Font font;
	sf::Text w_text;
	bool overlap_arrow = false;

public:
	bool directedGraph = false;
	bool weightedGraph = false;
	std::list<Edge>edges;

	void load(void);
	void createEdge(std::list<Vertex> &vertices, char *a, char *b, char *weight, bool usingAlpha);
	void removeRedundantEdge(int DeletedVertexNumber);
	void updateEdge(std::list<Vertex> &vertices);
	void deleteEdge(std::list<Vertex> &vertices, char *a, char *b, bool usingAlpha);
	void synchronizeEdgeIncidents(bool isStartWithZero);
	void changeEdgeColour(sf::Color colour);
	void changeWeightColour(sf::Color colour);
	void changeEdgeSize(float size);
	void changeWeightFontSize(float size);
	void reset(void);

	void renderEdge(sf::RenderWindow &window);
};

