#include "preProcessing/include/adjacency.h"
#include "include/MTDS.h"
#include <vector>
#include <set>
#include <iostream>
#include <queue>

using namespace std;

// BFS to find one connected component
set<int> bfs_component(const vector<vector<int>> &adj, int start, vector<bool> &visited) {
    set<int> component;
    queue<int> q;
    q.push(start);
    visited[start] = true;

    while (!q.empty()) {
        int u = q.front(); q.pop();
        component.insert(u);
        for (int v : adj[u]) {
            if (!visited[v]) {
                visited[v] = true;
                q.push(v);
            }
        }
    }

    return component;
}

int main() {
    vector<vector<int>> *adjacency = new vector<vector<int>>();
    string filename = "graphs/testGraph2.edges";  // adjust if needed
    build_adjacency_list(filename, adjacency);

    vector<bool> visited(adjacency->size(), false);
    vector<set<int>> all_subgraphs;

    for (int i = 0; i < adjacency->size(); ++i) {
        if (!visited[i] && !adjacency->at(i).empty()) {
            set<int> component = bfs_component(*adjacency, i, visited);

            // Take first 3 nodes as seed
            set<int> seed;
            int count = 0;
            for (int node : component) {
                seed.insert(node);
                if (++count >= 3) break;
            }

            // Run triangle-dense detection
            set<int> subgraph = locally_optimal_triangle_dense_subgraph(*adjacency, adjacency->size(), seed, 0.1);

            if (!subgraph.empty()) {
                all_subgraphs.push_back(subgraph);
            }
        }
    }

    // 🔽 Final summary output
    cout << "\n=== All Maximal Triangle-Dense Subgraphs ===\n";
    int idx = 1;
    for (const auto& sub : all_subgraphs) {
        cout << "Subgraph " << idx++ << ": ";
        for (int node : sub) {
            cout << node << " ";
        }
        cout << "\n";
    }

    return 0;
}
