#pragma once
#include <SFML/Graphics.hpp>
class Edge
{
public:
	sf::RectangleShape shape;
	sf::Sprite direction;
	sf::Text weight_text;
	int weight;
	int v1, v2;
	bool RM_FLAG, draw_ok;

	Edge();
};

