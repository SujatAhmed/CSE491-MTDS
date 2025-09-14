#include <algorithm>
#include <map>
#include <set>
#include <vector>
#include "../include/incrementalTriangleCount.h"
using namespace std;

// Returns the number of triangles created in the subgraph when adding new_node
int incremental_triangle_count(set<int> &subgraph,
                              const map<int, vector<int>> graph,
                              int new_node) {
    int triangle_count = 0;

    // Find neighbors of new_node that are in the subgraph
    vector<int> valid_neighbors;
    auto it = graph.find(new_node);
    if (it != graph.end()) {
        for (int v : it->second) {
            if (subgraph.count(v)) {
                valid_neighbors.push_back(v);
            }
        }
    }

    // For each unordered pair of valid neighbors, check if they are connected in the subgraph
    int n = valid_neighbors.size();
    for (int i = 0; i < n; ++i) {
        int u = valid_neighbors[i];
        auto uit = graph.find(u);
        if (uit == graph.end()) continue;
        for (int j = i + 1; j < n; ++j) {
            int v = valid_neighbors[j];
            // Are u and v connected in the full graph (and thus in subgraph if both are present)?
            if (find(uit->second.begin(), uit->second.end(), v) != uit->second.end()) {
                triangle_count++;
            }
        }
    }
    return triangle_count;
}
