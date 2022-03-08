#include "VisualizationManager.hpp"

VisualizationManager::VisualizationManager(void)
{
	traverseTex.loadFromFile("res/traversal.png");
	traverseImage.setTexture(traverseTex);
	//traverseImage.setScale(1.5, 1.5);
	traverseImage.setPosition(0, 610);
	traverseText.setPosition(230, 610);
	traverseStatusTex.loadFromFile("res/traverse_status.png");
	traverseStatus.setTexture(traverseStatusTex);
	traverseStatus.setPosition(920, 10);

	tex.loadFromFile("res/inprogress.png");
	statusImage.setTexture(tex);
	statusImage.setPosition(900, 10);
	font.loadFromFile("res/my_font.ttf");
	statusText.setFont(font);
	statusText.setCharacterSize(20.f);
	traverseText.setFont(font);
	traverseText.setCharacterSize(20.f);
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
	traverse.clear();
	for (int i = 0; i < maxSize; i++)
		visited[i] = false;
	executed = false;
	show_statusText = false;
}

void VisualizationManager::bfs(int(&adjMatrix)[maxSize][maxSize], VertexManager &v, EdgeManager &e, int baseIndex, float dt)
{
	if (!executed) {
		s = baseIndex;
		visited[s] = true;
		queue.clear();
		queue.push_back(s);
		executed = true;
	}

	int x = queue.front();
	//std::cout << x << " ";

	for (auto k = v.vertices.begin(); k != v.vertices.end(); k++) {
		if (k->numbering == x) {
			k->shape.setFillColor(highlightColour);
			traverse.push_back(*k);
		}	
	}

	queue.pop_front();

	for (int i = baseIndex; i <= v.current_vertices; i++)
	{
		if (adjMatrix[x][i] == 1 && visited[i] == false)
		{
			visited[i] = true;
			queue.push_back(i);
			//std::cout << i << " ";
		}
	}
}

void VisualizationManager::dfs(int(&adjMatrix)[maxSize][maxSize], VertexManager &v, EdgeManager &e, int baseIndex, float dt)
{
	if (!executed) {
		s = baseIndex;
		visited[s] = true;
		stack.clear();
		stack.push_back(s);
		traverse.push_back(v.vertices.front());
		executed = true;
	}

	int i;

	int x = stack.back();

	if (!visited[x]) {
		visited[x] = true;
		for (auto k = v.vertices.begin(); k != v.vertices.end(); k++) {
			if (k->numbering == x) {
				k->shape.setFillColor(highlightColour);
				traverse.push_back(*k);
			}
		}
	}
	

	for (i = baseIndex; i <= v.current_vertices; i++)
	{
		if (adjMatrix[x][i] == 1 && visited[i] == false)
		{
			stack.push_back(i);
			//visited[i] = true;
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


//this function gets the input for finding shortest paths
/////modification needed
	//only working for normal numbering and not with usingAlpha and StartFromZero
void VisualizationManager::getInputSP(char * a, char * b, bool usingAlpha, bool startFromZero)
{
	src = dest = 0;
	try {
		if (usingAlpha) {
			if (islower(*a) != 0)
				*a = toupper(*a);
			if (islower(*b) != 0)
				*b = toupper(*b);
			src = static_cast<int>(*a) - 65;
			dest = static_cast<int>(*b) - 65;
		}
		else {
			src = std::atoi(a);
			dest = std::atoi(b);
		}
	}
	catch (const std::invalid_argument &ex) {
		std::cout << ex.what();
	}

	//std::cout << src << " " << dest << "\n";

}

//this function uses bfs to find shortest path. Just a slight modification on the normal bfs
void VisualizationManager::bfsPathFind(int(&adjMatrix)[maxSize][maxSize], VertexManager & v, EdgeManager & e, int baseIndex)
{
	if (!executed) {
		visited[src] = true;
		queue.clear();
		queue.push_back(src);
		executed = true;
	}

	int x = queue.front();
	//traverse.push_back(x);
	queue.pop_front();

	for (int i = baseIndex; i <= v.current_vertices; i++)
	{
		if (adjMatrix[x][i] == 1 && visited[i] == false)
		{
			visited[i] = true;
			queue.push_back(i);
			pred[i] = x;
		}
	}

	pred[src] = -1;
	
}

//this function is to get the right path after using bfsPathFind
void VisualizationManager::getPath(int * pred, VertexManager &v)
{
	showingTraverse = true;
	int i = dest, pathLength = 0;
	while (pred[i] != -1) {
		path[pathLength++] = i;
		i = pred[i];
	}
	path[pathLength++] = i;
	//now reverse the path as it was already in reversed order for ease of calculation
	std::reverse(path, path + pathLength);

	//highlight the vertices
	std::string pathString = "";
	for (int i = 0; i < pathLength; i++) {
		for (auto k = v.vertices.begin(); k != v.vertices.end(); k++) {
			if (k->numbering == path[i]) {
				k->shape.setFillColor(highlightColour);
				std::stringstream sstream;
				if (v.usingAlpha) {
					std::string name = k->name.getString();
					sstream << name.c_str();
				}
				else
					sstream << k->numbering;
				pathString.append(sstream.str());
				break;
			}
		}
		if (i < pathLength-1)
			pathString.append("->");
	}

	traverseText.setString(pathString);
}

//this is for getting the traverse string after BFS or DFS
void VisualizationManager::getTraversed(void)
{
	onProgress = false;
	showingTraverse = true;
	std::string traverseString = "";
	std::string temp = "";
	for (auto i = traverse.begin(); i != traverse.end(); i++) {
		std::stringstream sstream;
		temp = i->name.getString();	//get the string form sf::Text
		sstream << temp.c_str();	//pass the constant string
		traverseString.append(sstream.str());
		//if(traverse.back() != *i)
			traverseString.append("->");
	}

	traverseText.setString(traverseString);
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

	if(showingTraverse){
		window.draw(traverseStatus);
		window.draw(traverseImage);
		window.draw(traverseText);
	}

	if (show_statusText)
		window.draw(statusText);
}
