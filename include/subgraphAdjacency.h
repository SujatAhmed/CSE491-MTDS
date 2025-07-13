

#ifndef SUBGRAPH_ADJACENCY_H
#define SUBGRAPH_ADJACENCY_H

#include <vector>
#include <set>

// Generates the adjacency list of a subgraph defined by SubgraphNodes,
// using the adjacency list of the full graph.
// Includes console output for debugging.
std::vector<std::vector<int>> get_subgraph_adjacency(
    const std::vector<std::vector<int>>& adj,
    const std::set<int>& SubgraphNodes
);

#endif // SUBGRAPH_ADJACENCY_H

