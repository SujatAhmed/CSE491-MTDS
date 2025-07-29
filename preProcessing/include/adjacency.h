
#ifndef ADJACENCY_H
#define ADJACENCY_H

#include <string>
#include <vector>
#include <map>
using namespace std;

// Builds an adjacency list from a graph file
// Parameters:
//   filename - path to the graph file (each line contains "u v" pairs)
//   adjacency - pointer to the adjacency list to be populated
// Notes:
//   - Creates an undirected graph (edges are added in both directions)
//   - Nodes are assumed to be zero-based or positive integers
map<int, vector<int>> generateAdjacencyMap(const std::string& filename);

#endif // ADJACENCY_H
