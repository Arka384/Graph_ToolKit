#pragma once
#include "File_IO.hpp"
class VisualizationManager
{
private:
	sf::Sprite statusImage;
	sf::Texture tex;
	sf::Text statusText;
	sf::Font font;
	std::string statusString = "Backtracking from: ";
	sf::Color highlightColour = sf::Color::Yellow;
	float timer = 0, timeLimit = 0.2;
	int s = 0;
	std::list<int> queue;
	std::list<int> stack;
	bool visited[20] = { false };
	bool executed = false;

public:
	int sleepTime = 1000;
	bool onProgress = false;
	bool show_statusText = false;

	VisualizationManager(void);
	void reset(VertexManager &v);
	void bfs(int(&adjMatrix)[20][20], VertexManager &v, EdgeManager &e, int baseIndex, float dt);
	void dfs(int(&adjMatrix)[20][20], VertexManager &v, EdgeManager &e, int baseIndex, float dt);
	void changeHighlightColour(sf::Color colour);
	void changeSpeed(int speed);
	int getQueueSize(void);
	int getStackSize(void);
	void displayStatus(sf::RenderWindow &window);
};

