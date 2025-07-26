
#include <iostream>
#include <set>
#include <vector>
#include "../include/subgraphAdjacency.h"
#include <unordered_map>
using namespace std;

vector<vector<int>> get_subgraph_adjacency(const vector<vector<int>> &adj,
                                           const set<int> &SubgraphNodes) {
    vector<int> nodes(SubgraphNodes.begin(), SubgraphNodes.end());
    int k = nodes.size();

    // Map global node ID to local index (0...k-1)
    unordered_map<int, int> globalToLocal;
    for (int i = 0; i < k; ++i) {
        globalToLocal[nodes[i]] = i;
    }

    // Create k x k adjacency matrix
    vector<vector<int>> sub_adj(k, vector<int>(k, 0));

    for (int i = 0; i < k; ++i) {
        int u_global = nodes[i];
        for (int v_global : adj[u_global]) {
            if (SubgraphNodes.count(v_global)) {
                int j = globalToLocal[v_global];
                sub_adj[i][j] = 1;
            }
        }
    }

    return sub_adj;
}

