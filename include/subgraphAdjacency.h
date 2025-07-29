

#ifndef SUBGRAPH_ADJACENCY_H
#define SUBGRAPH_ADJACENCY_H

#include <vector>
#include <set>
#include <map>
using namespace std;

// Generates the adjacency list of a subgraph defined by SubgraphNodes,
// using the adjacency list of the full graph.

map <int, vector<int>>  generateSubgraphAdjacencyMap(map <int, vector<int>> adjacencyMap,
                                                     set<int> subgraph);



#endif // SUBGRAPH_ADJACENCY_H

