#include "Vertex.hpp"

Vertex::Vertex()
{
	//nothing in here. HAHAHAHAHAH.....
}

Vertex::Vertex(sf::Text name, int numbering, sf::Color vertexColor, float vertexSize, bool isScattered)
{
	this->numbering = numbering;
	//	std::cout << this->numbering << "\n";
	this->name = name;
	shape.setRadius(vertexSize / 2);
	shape.setFillColor(vertexColor);
	shape.setOrigin(vertexSize / 2, vertexSize / 2);

	shape.setPosition(100, 100);
	if (isScattered)
		shape.setPosition(std::rand() % 900 + 100, std::rand() % 400 + 100);

	this->name.setPosition(this->shape.getPosition().x - 5, this->shape.getPosition().y - 5);

}
