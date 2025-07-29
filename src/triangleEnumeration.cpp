
#include "../include/triangleEnumeration.h"
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
