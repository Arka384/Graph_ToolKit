#include "EdgeManager.hpp"

void EdgeManager::load(void)
{
	font.loadFromFile("res/my_font.ttf");
	arrow_tex.loadFromFile("res/arrow.png");
	arrow.setTexture(arrow_tex);
	arrow.setTextureRect(sf::IntRect(0, 0, 256, 392));
	arrow.setScale(0.1, 0.1);
	w_text.setFont(font);
	w_text.setCharacterSize(weightSize);
	w_text.setFillColor(weightColour);
}

void EdgeManager::createEdge(std::list<Vertex> &vertices, char *a, char *b, char *weight, bool usingAlpha)
{
	try {
		if (usingAlpha) {
			if (islower(*a) != 0)
				*a = toupper(*a);
			if (islower(*b) != 0)
				*b = toupper(*b);

			e1 = static_cast<int>(*a) - 65;
			e2 = static_cast<int>(*b) - 65;
		}
		else {
			e1 = std::stoi(a);
			e2 = std::stoi(b);
		}
		copy_w = std::stoi(weight);
	}
	catch (const std::invalid_argument &e) {
		copy_w = 1;
	}

	//mulitple optimization checks for same edges with same incidents
	if (!directedGraph) {
		for (auto i = edges.begin(); i != edges.end(); i++)
			if ((i->v1 == e1 && i->v2 == e2) || (i->v1 == e2 && i->v2 == e1))
				return;
	}
	else {//if directed graph then make the arrow overlap
		for (auto i = edges.begin(); i != edges.end(); i++) {
			if (i->v1 == e2 && i->v2 == e1) {	//another edge exists
				overlap_arrow = true;
				break;
			}
		}
	}

	for (auto i = vertices.begin(); i != vertices.end(); i++) {
		if (i->numbering == e1) {
			for (auto j = vertices.begin(); j != vertices.end(); j++) {
				if (j->numbering == e2)
				{
					float dx = j->shape.getPosition().x - i->shape.getPosition().x;
					float dy = j->shape.getPosition().y - i->shape.getPosition().y;
					float dist = (dx*dx + dy*dy);
					dist = std::sqrt(dist);	//this is the distance between the two balls in which the edge is
					float angle = std::atan2(dy, dx);	//this is the angle of rotation for the edge to be inside the balls (yeah balls...)
					angle = angle * (180 / 3.1415);

					Edge temp;	//create an edge object then initialize and then add to existing edges list.
					
					std::stringstream ss;
					ss << copy_w;
					temp.weight_text = w_text;
					temp.weight_text.setString(ss.str());
					temp.weight_text.setFillColor(weightColour);
					temp.weight_text.setCharacterSize(weightSize);
					temp.weight_text.setOrigin(temp.weight_text.getGlobalBounds().width / 2, temp.weight_text.getGlobalBounds().height / 2);

					temp.direction = arrow;
					temp.direction.setRotation(angle);
					int Y_offset = (overlap_arrow) ? 210 : 130;
					temp.direction.setOrigin(temp.direction.getGlobalBounds().width / 2, temp.direction.getGlobalBounds().height / 2 + Y_offset);

					temp.shape.setRotation(angle);
					temp.shape.setPosition(i->shape.getPosition());	//I make the edge permanently connected to one vertex and free form to another
					temp.shape.setSize(sf::Vector2f(dist, edgeSize));	//this makes the size of the edges dynamic
					temp.shape.setFillColor(edgeColour);
					temp.v1 = e1;
					temp.v2 = e2;
					temp.weight = copy_w;
					temp.RM_FLAG = false;
					temp.draw_ok = (overlap_arrow) ? false : true;

					edges.push_back(temp);
					overlap_arrow = false;	//restore back
				}
			}
		}
	}
}

void EdgeManager::removeRedundantEdge(int DeletedVertexNumber)
{	//delets the redundent edge where there's no vertex associated with it.

	for (auto k = edges.begin(); k != edges.end(); k++) {
		if (k->v1 == DeletedVertexNumber || k->v2 == DeletedVertexNumber) {
			k->RM_FLAG = true;
		}
	}

	auto i = edges.begin();
	while (i != edges.end()) {
		if (i->RM_FLAG)
			i = edges.erase(i);
		else
			i++;
	}
}

void EdgeManager::updateEdge(std::list<Vertex>& vertices)
{
	for (auto k = edges.begin(); k != edges.end(); k++) {
		for (auto i = vertices.begin(); i != vertices.end(); i++) {
			if (i->numbering == k->v1) {
				for (auto j = vertices.begin(); j != vertices.end(); j++) {
					if (j->numbering == k->v2)
					{
						float dx = j->shape.getPosition().x - i->shape.getPosition().x;
						float dy = j->shape.getPosition().y - i->shape.getPosition().y;
						float dist = (dx*dx + dy * dy);
						dist = sqrt(dist);	//this is the distance between the two balls in which the edge is
						float angle = atan2(dy, dx);	//this is the angle of rotation for the edge to be inside the balls
						angle = angle * (180 / 3.1415);

						k->shape.setRotation(angle);
						k->shape.setPosition(i->shape.getPosition());	//I make the edge permanently connected to one vertex and free form to another
						k->shape.setSize(sf::Vector2f(dist, edgeSize));	//this makes the size of the edges dynamic
						k->direction.setRotation(angle);	//this makes the direction sprite to rotate dynamically

						float x = (j->shape.getPosition().x + i->shape.getPosition().x) / 2;
						float y = (j->shape.getPosition().y + i->shape.getPosition().y) / 2;
						k->weight_text.setPosition(x, y);

						x = (x + i->shape.getPosition().x) / 2;
						y = (y + i->shape.getPosition().y) / 2;
						k->direction.setPosition(x, y);

					}
				}
			}
		}
	}
}

void EdgeManager::deleteEdge(std::list<Vertex>& vertices, char * a, char * b, bool usingAlpha)
{
	int i1 = 0, i2 = 0;
	try {
		if (usingAlpha) {
			if (islower(*a) != 0)
				*a = toupper(*a);
			if (islower(*b) != 0)
				*b = toupper(*b);

			i1 = static_cast<int>(*a) - 65;
			i2 = static_cast<int>(*b) - 65;
		}
		else {
			i1 = std::stoi(a);
			i2 = std::stoi(b);
		}
	}
	catch (const std::invalid_argument &e) {
		//std::cout << "Exception\n";
	}

	if (!directedGraph) {
		for (auto k = edges.begin(); k != edges.end(); ) {
			if ((k->v1 == i1 && k->v2 == i2) || (k->v1 == i2 && k->v2 == i1))
				k = edges.erase(k);
			else
				k++;
		}
	}
	else {
		for (auto k = edges.begin(); k != edges.end(); ) {
			if ((k->v1 == i1 && k->v2 == i2)) {
				k = edges.erase(k);
				//to fix both the edges incident on same vertices are being deleted when only one of 'em is deleted
				for (auto i = edges.begin(); i != edges.end(); i++) {
					if ((i->v1 == i2 && i->v2 == i1))//the opposite condition of the above checked one
						if (!i->draw_ok) {
							i->draw_ok = true;
							i->direction.setOrigin(i->direction.getGlobalBounds().width / 2, i->direction.getGlobalBounds().height / 2 + 130);
						}
				}
				break;
			}
			else
				k++;
		}
	}
}

void EdgeManager::synchronizeEdgeIncidents(bool isStartWithZero)
{
	if (isStartWithZero) {
		for (auto i = edges.begin(); i != edges.end(); i++) {
			i->v1--;
			i->v2--;
		}
	}
	else {
		for (auto i = edges.begin(); i != edges.end(); i++) {
			i->v1++;
			i->v2++;
		}
	}
}

void EdgeManager::changeEdgeColour(sf::Color colour)
{
	edgeColour = colour;
	for (auto i = edges.begin(); i != edges.end(); i++)
		i->shape.setFillColor(edgeColour);
}

void EdgeManager::changeWeightColour(sf::Color colour)
{
	weightColour = colour;
	for (auto i = edges.begin(); i != edges.end(); i++)
		i->weight_text.setFillColor(weightColour);
}

void EdgeManager::changeEdgeSize(float size)
{
	edgeSize = size;
	for (auto i = edges.begin(); i != edges.end(); i++)
		i->shape.setSize(sf::Vector2f(i->shape.getSize().x, edgeSize));
}

void EdgeManager::changeWeightFontSize(float size)
{
	weightSize = size;
	for (auto i = edges.begin(); i != edges.end(); i++)
		i->weight_text.setCharacterSize(weightSize);
}

void EdgeManager::reset(void)
{
	edges.clear();
	e1 = e2 = 0;
	copy_w = 1;
}

void EdgeManager::renderEdge(sf::RenderWindow & window)
{
	for (auto i = edges.begin(); i != edges.end(); i++) {
		if (i->draw_ok)
			window.draw(i->shape);

		if (directedGraph)
			window.draw(i->direction);

		if (weightedGraph)
			window.draw(i->weight_text);
	}
}
