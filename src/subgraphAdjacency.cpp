#include <iostream>
#include <set>
#include <vector>
#include "../include/subgraphAdjacency.h"
#include <unordered_map>
using namespace std;

map<int, vector<int>> generateSubgraphAdjacencyMap(
    map<int, vector<int>> adjacencyMap,
    set<int> subgraph
) {
    map<int, vector<int>> subgraphMap;

    for (int node : subgraph) {
        // Skip if node is not in the original adjacency map
        if (adjacencyMap.find(node) == adjacencyMap.end()) continue;

        vector<int> filteredNeighbors;
        for (int neighbor : adjacencyMap[node]) {
            // Only include neighbors also in the subgraph
            if (subgraph.count(neighbor)) {
                filteredNeighbors.push_back(neighbor);
            }
        }

        // Only add node if it has any neighbors in the subgraph
        if (!filteredNeighbors.empty()) {
            subgraphMap[node] = filteredNeighbors;
        }
    }

    return subgraphMap;
}
