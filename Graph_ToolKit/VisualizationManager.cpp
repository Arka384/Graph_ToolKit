#include "VisualizationManager.hpp"

VisualizationManager::VisualizationManager(void)
{
	tex.loadFromFile("res/inprogress.png");
	statusImage.setTexture(tex);
	statusImage.setPosition(800, 20);
	font.loadFromFile("res/my_font.ttf");
	statusText.setFont(font);
	statusText.setCharacterSize(20.f);
	//to make non zero size
	queue.push_back(s);
	stack.push_back(s);
}

void VisualizationManager::reset(VertexManager &v)
{
	for (auto k = v.vertices.begin(); k != v.vertices.end(); k++)
		k->shape.setFillColor(v.vertexColor);
	queue.push_back(s);
	stack.push_back(s);
	for (int i = 0; i < 20; i++)
		visited[i] = false;
	executed = false;
	onProgress = false;
	show_statusText = false;
}

void VisualizationManager::bfs(int(&adjMatrix)[20][20], VertexManager &v, EdgeManager &e, int baseIndex, float dt)
{
	if (!executed) {
		s = baseIndex;
		visited[s] = true;
		queue.clear();
		queue.push_back(s);
		executed = true;
	}

	int x = queue.front();

	for (auto k = v.vertices.begin(); k != v.vertices.end(); k++) {
		if (k->numbering == x)
			k->shape.setFillColor(highlightColour);
	}

	queue.pop_front();

	for (int i = baseIndex; i <= v.current_vertices; i++)
	{
		if (adjMatrix[x][i] == 1 && visited[i] == false)
		{
			visited[i] = true;
			queue.push_back(i);
		}
	}
}

void VisualizationManager::dfs(int(&adjMatrix)[20][20], VertexManager &v, EdgeManager &e, int baseIndex, float dt)
{
	if (!executed) {
		s = baseIndex;
		visited[s] = true;
		stack.clear();
		stack.push_back(s);
		executed = true;
	}

	int i;

	int x = stack.back();

	for (auto k = v.vertices.begin(); k != v.vertices.end(); k++) {
		if (k->numbering == x)
			k->shape.setFillColor(highlightColour);
	}

	for (i = baseIndex; i <= v.current_vertices; i++)
	{
		if (adjMatrix[x][i] == 1 && visited[i] == false)
		{
			stack.push_back(i);
			visited[i] = true;
			break;
		}
	}
	if (i == v.current_vertices + 1) {
		show_statusText = true;
		int t = stack.back();
		stack.pop_back();
		std::stringstream ss;
		if (v.usingAlpha)
			ss << (char)(t + 65);
		else
			ss << t;
		statusString.append(ss.str());
		statusText.setString(statusString);
		statusString = "Backtracking from: ";
	}

}

void VisualizationManager::changeHighlightColour(sf::Color colour)
{
	highlightColour = colour;
}

void VisualizationManager::changeSpeed(int speed)
{
	sleepTime = speed;
}

int VisualizationManager::getQueueSize(void)
{
	return queue.size();
}

int VisualizationManager::getStackSize(void)
{
	return stack.size();
}

void VisualizationManager::displayStatus(sf::RenderWindow & window)
{
	if (onProgress) {
		window.draw(statusImage);
	}
	if (show_statusText)
		window.draw(statusText);
}
