#include "VertexManager.hpp"

void VertexManager::load(void)
{
	font.loadFromFile("res/my_font.ttf");
	nameText.setFont(font);
	nameText.setCharacterSize(font_size);
}

void VertexManager::createVertex(void)
{
	//set the new number for the vertex and send it to vertex constructor
	if (vertices.size() == 0 && startFromZero)
		current_vertices = -1;

	current_vertices++;

	std::stringstream ss;
	if (usingAlpha)
		ss << (char)(current_vertices + base);
	else
		ss << current_vertices;

	nameText.setFillColor(nameTextColor);
	nameText.setCharacterSize(font_size);
	nameText.setString(ss.str());
	nameText.setOrigin(nameText.getGlobalBounds().width / 2, nameText.getGlobalBounds().height / 2);

	Vertex v(nameText, current_vertices, vertexColor, vertex_size, isScattered);
	vertices.push_back(v);
}

int VertexManager::deleteVertex(void)
{
	if (vertices.size() == 0)
		return -1;
	current_vertices--;
	int x = vertices.back().numbering;
	vertices.pop_back();
	return x;
}

void VertexManager::changeNaming(void)
{
	if (usingAlpha) {
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			std::stringstream ss;
			ss << static_cast<char>(i->numbering + base);
			i->name.setString(ss.str());
		}
	}
	else {
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			std::stringstream ss;
			ss << static_cast<int>(i->numbering);
			i->name.setString(ss.str());
		}
	}
	lastUsingAlpha = usingAlpha;
}

void VertexManager::changeNumbering(bool &EdgeSynch)
{
	if (lastNumberChange == startFromZero)
		return;
	int x = 0, y = 1;
	if (startFromZero) {
		current_vertices--;
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			i->numbering = x++;
		}
	}
	else {
		current_vertices++;
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			i->numbering = y++;
		}
	}
	lastNumberChange = startFromZero;
	lastUsingAlpha = !lastUsingAlpha;
	EdgeSynch = true;
}

void VertexManager::updateVertices(sf::RenderWindow &window)
{
	int x = -1;
	sf::Vector2i mousePos = sf::Mouse::getPosition(window);
	if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
	{
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			i->name.setPosition(i->shape.getPosition().x - 5, i->shape.getPosition().y - 5);
			if (mousePos.x > i->shape.getPosition().x - vertex_size / 2 && mousePos.x < i->shape.getPosition().x + vertex_size / 2 &&
				mousePos.y > i->shape.getPosition().y - vertex_size / 2 && mousePos.y < i->shape.getPosition().y + vertex_size / 2)
				x = i->numbering;
		}

		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			if (i->numbering == x)
				i->shape.setPosition(mousePos.x, mousePos.y);
		}
	}
}


void VertexManager::changeVertexColor(sf::Color color)
{
	vertexColor = color;
	for (auto i = vertices.begin(); i != vertices.end(); i++)
		i->shape.setFillColor(vertexColor);
}

void VertexManager::changeVertexSize(float size)
{
	vertex_size = size;
	for (auto i = vertices.begin(); i != vertices.end(); i++) {
		i->shape.setRadius(vertex_size / 2);
		i->shape.setOrigin(vertex_size / 2, vertex_size / 2);
	}
}

void VertexManager::changeNameTextColor(sf::Color color)
{
	nameTextColor = color;
	for (auto i = vertices.begin(); i != vertices.end(); i++)
		i->name.setFillColor(nameTextColor);
}

void VertexManager::changeNameFontSize(float size)
{
	font_size = size;
	for (auto i = vertices.begin(); i != vertices.end(); i++) {
		i->name.setCharacterSize(size);
		i->name.setOrigin(nameText.getGlobalBounds().width / 2, nameText.getGlobalBounds().height / 2);
	}
}

void VertexManager::reset(void)
{
	vertices.clear();
	current_vertices = 0;
}

void VertexManager::renderVertices(sf::RenderWindow &window)
{
	for (auto i = vertices.begin(); i != vertices.end(); i++) {
		window.draw(i->shape);
		window.draw(i->name);
	}
}
