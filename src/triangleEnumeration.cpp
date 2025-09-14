
#include "../include/triangleEnumeration.h"
#include <algorithm>
#include <set>
#include <vector>
using namespace std;

int bruteForceTriangleCounting(map<int, vector<int>> adjacencyMap) {
  int triangleCount = 0;

  // Convert adjacency lists to sets for faster lookup
  map<int, set<int>> adjSet;
  for (const auto &[node, neighbors] : adjacencyMap) {
    adjSet[node] = set<int>(neighbors.begin(), neighbors.end());
  }

  for (const auto &[u, neighborsU] : adjacencyMap) {
    for (int v : neighborsU) {
      if (u < v) { // avoid duplicate edge directions
        for (int w : adjacencyMap.at(v)) {
          if (v < w && adjSet[u].count(w)) {
            // (u, v), (v, w), and (w, u) exist
            triangleCount++;
          }
        }
      }
    }
  }

  return triangleCount;
}

int countTrianglesForNode(const map<int, vector<int>>& subgraphAdjacency, int node) {
    if (subgraphAdjacency.count(node) == 0) return 0;

    const vector<int>& neighbors = subgraphAdjacency.at(node);
    int triangleCount = 0;

    // For each pair (node, u), check how many common neighbors they share
    for (int u : neighbors) {
        if (subgraphAdjacency.count(u) == 0) continue;

        const vector<int>& u_neighbors = subgraphAdjacency.at(u);

        // Count common neighbors of node and u (excluding node and u themselves)
        vector<int> intersection;
        set_intersection(
            neighbors.begin(), neighbors.end(),
            u_neighbors.begin(), u_neighbors.end(),
            back_inserter(intersection)
        );

        triangleCount += intersection.size();
    }

    // Each triangle is counted 2 times (once with (node,u), once with (node,w))
    return triangleCount / 2;
}

