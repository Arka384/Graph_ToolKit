#pragma once
#include "Vertex.hpp"
#include <iostream>
#include <sstream>
#include <list>

class VertexManager
{
private:
	float vertex_size = 40.f;
	float font_size = 30.f;
	sf::Font font;
	sf::Text nameText;
	int base = 65;
	int startValue = 0;
	bool lastUsingAlpha = false;
	bool lastNumberChange = false;

public:
	int current_vertices = 0;
	bool usingAlpha = false;
	bool startFromZero = false;
	bool isScattered = false;
	std::list<Vertex> vertices;
	sf::Color vertexColor = sf::Color::Red, nameTextColor = sf::Color::Black;

	void load(void);
	void createVertex(void);
	int deleteVertex(void);
	void changeNaming(void);
	void changeNumbering(bool &EdgeSynch);
	void changeVertexColor(sf::Color color);
	void changeVertexSize(float size);
	void changeNameTextColor(sf::Color color);
	void changeNameFontSize(float size);
	void reset(void);
	void updateVertices(sf::RenderWindow &window);
	void renderVertices(sf::RenderWindow &window);
};

