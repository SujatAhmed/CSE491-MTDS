
#include <iostream>
#include <set>
#include <vector>
#include "../include/subgraphAdjacency.h"
using namespace std;

vector<vector<int>> get_subgraph_adjacency(const vector<vector<int>> &adj,
                                           const set<int> &SubgraphNodes) {
  int n = adj.size();
  vector<vector<int>> sub_adj(n); // same size as original to preserve indices

  // cout << "Building subgraph adjacency...\n";
  // cout << "Subgraph nodes: ";
  // for (int u : SubgraphNodes)
    // cout << u << " ";
  // cout << "\n\n";

  for (int u : SubgraphNodes) {
    // cout << "Checking neighbors of node " << u << ": ";
    // for (int v : adj[u])
    //   cout << v << " ";
    // cout << "\n";
    for (int v : adj[u]) {
      if (SubgraphNodes.count(v)) {
        sub_adj[u].push_back(v);
        // cout << "  --> adding edge " << u << " - " << v << "\n";
      }
    }
    // cout << "\n";
  }

  cout << "\nFinal Subgraph Adjacency List:\n";
  for (int i = 0; i < n; ++i) {
    if (!sub_adj[i].empty()) {
      cout << i << ": ";
      for (int v : sub_adj[i])
        cout << v << " ";
      cout << "\n";
    }
  }

  return sub_adj;
}
