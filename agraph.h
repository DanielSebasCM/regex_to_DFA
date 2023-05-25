/*
 * graph.h
 *
 *  Created on: 3/11/2018
 *      Author: pperezm
 */

#ifndef AGRAPH_H_
#define AGRAPH_H_

#include <sstream>
#include <set>
#include <map>
#include <list>
#include <stack>
#include <queue>
#include <vector>
#include <iostream>
#include <algorithm>

/***********************************************************/
/************************ UListGraph ***********************/
/***********************************************************/
#define EPSILON 'E'

class AutomataGraph
{
private:
	int start;
	std::set<int> final;
	int next;
	std::set<int> vertexes;
	std::map<int, std::set<std::pair<int, char>>> edges;
	void addVertex(int v);

public:
	AutomataGraph();
	AutomataGraph(const AutomataGraph &);

	void addEdge(int from, int to, char l);

	bool containsVertex(int v) const;

	std::set<int> getNeighbours(int) const;
	std::set<int> eClosure(int) const;
	std::set<int> eClosure(int, std::set<int>) const;
	std::set<int> eClosure(std::set<int>) const;

	char getWeight(int from, int to) const;

	int createVertex();

	std::pair<int, int> connectGraphToVertex(AutomataGraph g, int v);

	void setStart(int s) { start = s; }
	int getStart() const { return start; }

	void addFinal(int f) { final.insert(f); }
	void setFinal(std::set<int> e) { final = e; }
	std::set<int> getFinal() const { return final; }

	int getNext() { return next; }

	std::vector<int> getVertexes() const;
	std::map<int, std::set<std::pair<int, char>>> getEdges() const { return edges; }

	std::string toString() const;
};

AutomataGraph::AutomataGraph()
{
	next = 0;
}

AutomataGraph::AutomataGraph(const AutomataGraph &g)
{
	start = g.start;
	final = g.final;
	next = g.next;
	vertexes = g.vertexes;
	edges = g.edges;
}

void AutomataGraph::addEdge(int from, int to, char l)
{
	if (vertexes.find(from) == vertexes.end())
		addVertex(from);

	if (vertexes.find(to) == vertexes.end())
		addVertex(to);

	edges[from].insert(std::pair<int, char>(to, l));
}

void AutomataGraph::addVertex(int v)
{
	if (vertexes.find(v) == vertexes.end())
	{
		vertexes.insert(v);
		edges[v] = std::set<std::pair<int, char>>();
	}
}

bool AutomataGraph::containsVertex(int v) const
{
	return (vertexes.find(v) != vertexes.end());
}

std::set<int> AutomataGraph::getNeighbours(int v) const
{
	std::set<std::pair<int, char>> neighbours(edges.at(v));
	std::set<int> result;
	for (auto v2 : neighbours)
	{
		result.insert(v2.first);
	}

	return result;
}

std::set<int> AutomataGraph::eClosure(int v) const
{
	std::set<int> visited;
	return eClosure(v, visited);
}

std::set<int> AutomataGraph::eClosure(int v, std::set<int> visited) const
{
	std::set<int> result;
	result.insert(v);
	visited.insert(v);
	for (auto v2 : edges.at(v))
	{
		if (v2.second == EPSILON && visited.find(v2.first) == visited.end())
		{
			std::set<int> aux = eClosure(v2.first, visited);
			result.insert(aux.begin(), aux.end());
		}
	}

	return result;
}

std::set<int> AutomataGraph::eClosure(std::set<int> v) const
{
	std::set<int> result;
	for (auto v2 : v)
	{
		std::set<int> aux = eClosure(v2);
		result.insert(aux.begin(), aux.end());
	}

	return result;
}

char AutomataGraph::getWeight(int from, int to) const
{
	for (auto v : edges.at(from))
	{
		if (v.first == to)
			return v.second;
	}

	return NULL;
}

int AutomataGraph::createVertex()
{
	int v = next;
	next++;
	addVertex(v);
	return v;
}

std::pair<int, int> AutomataGraph::connectGraphToVertex(AutomataGraph g, int v)
{
	std::vector<int> vertexes = g.getVertexes();
	std::map<int, int> newVertexes;

	for (auto vOld : vertexes)
	{
		if (vOld == g.getStart())
		{
			newVertexes[vOld] = v;
			continue;
		}
		else
		{
			newVertexes[vOld] = createVertex();
		}
	}

	for (auto vFrom : vertexes)
	{
		for (auto vTo : g.getNeighbours(vFrom))
		{
			addEdge(newVertexes[vFrom], newVertexes[vTo], g.getWeight(vFrom, vTo));
		}
	}

	return std::pair<int, int>(newVertexes[g.getStart()], newVertexes[*g.getFinal().begin()]);
}

std::string AutomataGraph::toString() const
{
	std::stringstream aux;

	for (auto v : vertexes)
	{
		aux << (v) << "\t";
		for (auto v2 : edges.at(v))
		{
			aux << "\t"
				<< "(" << v2.first << "," << v2.second << ")";
		}
		aux << "\n";
	}
	aux << "\n";
	return aux.str();
}

std::vector<int> AutomataGraph::getVertexes() const
{
	std::vector<int> result(vertexes.begin(), vertexes.end());
	return result;
}

/***********************************************************/
/**************************** DFS **************************/
/***********************************************************/

std::vector<int> dfs(const int &start,
					 const AutomataGraph *graph)
{

	std::vector<int> visited;
	std::stack<int> pending;
	typename std::set<int>::iterator itr;

	pending.push(start);
	while (!pending.empty())
	{
		int v = pending.top();
		pending.pop();
		if (std::find(visited.begin(), visited.end(), v) == visited.end())
		{
			visited.push_back(v);
			std::set<int> connected =
				graph->getNeighbours(v);
			for (itr = connected.begin();
				 itr != connected.end(); itr++)
			{
				pending.push((*itr));
			}
		}
	}
	return visited;
}

/***********************************************************/
/**************************** BFS **************************/
/***********************************************************/

std::vector<int> bfs(const int &start, const AutomataGraph *graph)
{
	std::vector<int> visited;
	std::queue<int> pending;
	typename std::set<int>::iterator itr;

	pending.push(start);
	while (!pending.empty())
	{
		int v = pending.front();
		pending.pop();
		if (std::find(visited.begin(), visited.end(), v) == visited.end())
		{
			visited.push_back(v);
			std::set<int> connected =
				graph->getNeighbours(v);

			for (itr = connected.begin();
				 itr != connected.end(); itr++)
			{
				pending.push((*itr));
			}
		}
	}
	return visited;
}

#endif

// 0 - a
// 1 - b
// 2 - c
// 3 - d
// 4 - e
// 5 - f
// 6 - g
// 7 - h
// 8 - i