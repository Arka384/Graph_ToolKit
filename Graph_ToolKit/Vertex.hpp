#pragma once
#include <SFML/Graphics.hpp>
#include <iostream>
class Vertex
{
private:
	sf::Vector2f pos = sf::Vector2f(100, 100);
public:
	sf::CircleShape shape;
	sf::Text name;
	int numbering;

	Vertex();
	Vertex(sf::Text name, int numbering, sf::Color vertexColor, float vertexSize, bool isScattered);

};

