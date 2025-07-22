#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <ctime>
#include <fstream>

using namespace std;

const int MAX_TRIES = 10000;

vector<vector<int>> adj_list;
set<pair<int, int>> edge_set;  // to avoid duplicate edges
mt19937 rng(time(0));

// Count triangles in a subgraph
int count_triangles(const vector<vector<int>>& adj_list, const vector<int>& nodes) {
    unordered_set<int> node_set(nodes.begin(), nodes.end());
    int count = 0;

    for (int u : nodes) {
        for (int v : adj_list[u]) {
            if (v <= u || !node_set.count(v)) continue;

            const auto& adj_u = adj_list[u];
            const auto& adj_v = adj_list[v];
            int i = 0, j = 0;
            while (i < adj_u.size() && j < adj_v.size()) {
                if (!node_set.count(adj_u[i])) { ++i; continue; }
                if (!node_set.count(adj_v[j])) { ++j; continue; }
                if (adj_u[i] == adj_v[j]) {
                    if (adj_u[i] != u && adj_u[i] != v)
                        ++count;
                    ++i; ++j;
                } else if (adj_u[i] < adj_v[j]) {
                    ++i;
                } else {
                    ++j;
                }
            }
        }
    }

    return count / 3;
}

// Add undirected edge if it doesn’t exist
bool add_edge(int u, int v) {
    if (u == v) return false;
    if (u > v) swap(u, v);
    if (edge_set.count({u, v})) return false;
    edge_set.insert({u, v});
    adj_list[u].push_back(v);
    adj_list[v].push_back(u);
    return true;
}

// Implant triangle-dense subgraph
void implant_triangle_dense_subgraph(int N, int subgraph_size, double threshold) {
    uniform_int_distribution<int> dist(0, N - 1);
    vector<int> nodes;

    // Pick unique subgraph nodes
    unordered_set<int> chosen;
    while (nodes.size() < subgraph_size) {
        int x = dist(rng);
        if (chosen.insert(x).second)
            nodes.push_back(x);
    }

    // Try adding random edges until triangle density ≥ threshold
    int tries = 0;
    while (tries < MAX_TRIES) {
        // Shuffle and add random edge
        int u = nodes[rng() % subgraph_size];
        int v = nodes[rng() % subgraph_size];
        if (add_edge(u, v)) {
            ++tries;
            for (int node : nodes)
                sort(adj_list[node].begin(), adj_list[node].end()); // keep sorted for set intersection

            int triangles = count_triangles(adj_list, nodes);
            double density = (double)triangles / subgraph_size;

            if (density >= threshold) {
                cout << "✅ Implanted subgraph with density " << density << " (" << triangles << " triangles)\n";
                return;
            }
        }
    }

    cout << "⚠️ Failed to reach threshold in max tries.\n";
}

int main() {
    int N, choice;
    int min_nodes = 100, max_nodes = 300;
    int T;
    double threshold;

    cout << "Choose graph size option:\n";
    cout << "1. Set number of nodes manually\n";
    cout << "2. Randomize number of nodes (" << min_nodes << "-" << max_nodes << ")\n";
    cin >> choice;

    if (choice == 1) {
        cout << "Enter number of nodes: ";
        cin >> N;
    } else {
        uniform_int_distribution<int> dist(min_nodes, max_nodes);
        N = dist(rng);
        cout << "Randomized number of nodes: " << N << "\n";
    }

    adj_list.resize(N);

    cout << "Enter number of triangle-dense subgraphs to implant: ";
    cin >> T;

    cout << "Enter triangle density threshold (e.g., 1.5): ";
    cin >> threshold;

    int subgraph_size = 10;  // fixed for now, can be input later

    // Start implanting
    for (int i = 0; i < T; ++i) {
        cout << "\nImplanting dense subgraph #" << i + 1 << "...\n";
        implant_triangle_dense_subgraph(N, subgraph_size, threshold);
    }

    // Output final edges
    ofstream outfile("synthetic_graph.edges");
    if (!outfile) {
        cerr << "❌ Failed to open output file.\n";
        return 1;
    }

    outfile << "# Nodes: " << N << ", Edges: " << edge_set.size() << "\n";
        for (auto& [u, v] : edge_set) {
            outfile << u << " " << v << "\n";
        }

    cout << "\n✅ Saved edge list to synthetic_graph.edges\n";

    return 0;
}
