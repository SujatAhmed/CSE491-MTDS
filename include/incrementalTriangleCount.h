#ifndef INCREMENTAL_TRIANGLE_COUNT_H
#define INCREMENTAL_TRIANGLE_COUNT_H

#include <map>
#include <vector>
#include <set>
using namespace std;

// Returns the number of triangles created in the subgraph when adding new_node.
// - subgraph: set of integers representing the subgraph.
// - graph: adjacency map of the whole graph. (node-> neighbors)
// - new_node: node to be added
int incremental_triangle_count(set<int> &subgraph,
                              const map<int, vector<int>> graph,
                              int new_node);
#endif // INCREMENTAL_TRIANGLE_COUNT_H
