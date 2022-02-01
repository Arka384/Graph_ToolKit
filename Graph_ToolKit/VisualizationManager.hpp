#pragma once
#include "File_IO.hpp"
class VisualizationManager
{
private:
	sf::Sprite statusImage, traverseImage, traverseStatus;
	sf::Texture tex, traverseTex, traverseStatusTex;
	sf::Text statusText, traverseText;
	sf::Font font;
	std::string statusString = "Backtracking from: ";
	sf::Color highlightColour = sf::Color::Yellow;
	float timer = 0, timeLimit = 0.2;
	int s = 0;
	std::list<int> queue;
	std::list<int> stack;
	static const int maxSize = GL_ArraySize;
	bool visited[maxSize] = { false };
	bool executed = false;

public:
	int sleepTime = 1000;
	bool onProgress = false;
	bool showingTraverse = false;
	bool show_statusText = false;
	std::list<int> traverse;

	VisualizationManager(void);
	void reset(VertexManager &v);
	void bfs(int(&adjMatrix)[maxSize][maxSize], VertexManager &v, EdgeManager &e, int baseIndex, float dt);
	void dfs(int(&adjMatrix)[maxSize][maxSize], VertexManager &v, EdgeManager &e, int baseIndex, float dt);
	void getTraversed(void);
	void changeHighlightColour(sf::Color colour);
	void changeSpeed(int speed);
	int getQueueSize(void);
	int getStackSize(void);
	void displayStatus(sf::RenderWindow &window);
};

