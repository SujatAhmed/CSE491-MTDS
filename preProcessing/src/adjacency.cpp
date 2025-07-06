
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;

void build_adjacency_list(const string &filename, vector<vector<int>> *adjacency) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    string line;
    int u, v;
    int max_node = 0;

    vector<pair<int, int>> edges;

    while (getline(file, line)) {
        istringstream iss(line);
        if (!(iss >> u >> v)) {
            cerr << "Skipping malformed line: " << line << "\n";
            continue;
        }

        cout << "Read edge: " << u << " " << v << "\n";
        max_node = max({max_node, u, v});
        edges.emplace_back(u, v);
    }

    cout << "Max node ID: " << max_node << "\n";

    adjacency->assign(max_node + 1, vector<int>());

    for (const auto &edge : edges) {
        int u = edge.first;
        int v = edge.second;

        // Add both directions for undirected graph
        (*adjacency)[u].push_back(v);
        (*adjacency)[v].push_back(u);
    }

    cout << "\nAdjacency List:\n";
    for (size_t i = 0; i < adjacency->size(); ++i) {
        cout << i << ": ";
        for (int neighbor : (*adjacency)[i]) {
            cout << neighbor << " ";
        }
        cout << "\n";
    }
}

