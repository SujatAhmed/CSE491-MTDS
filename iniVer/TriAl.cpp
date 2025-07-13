#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <sstream>
#include <string>
using namespace std;

bool degree_cmp(const pair<int, int>& a, const pair<int, int>& b) {
    return (a.second < b.second) || (a.second == b.second && a.first < b.first);
}

int countTriangles(int n, const vector<pair<int, int>>& edges) {
    vector<vector<int>> adj(n);
    vector<int> degree(n, 0);

    // Build adjacency list and compute degree
    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        degree[u]++;
        degree[v]++;
    }

    // Order nodes by degree
    vector<int> new_id(n);
    vector<pair<int, int>> nodes(n);
    for (int i = 0; i < n; ++i) nodes[i] = {i, degree[i]};
    sort(nodes.begin(), nodes.end(), degree_cmp);
    for (int i = 0; i < n; ++i) new_id[nodes[i].first] = i;

    // Rebuild adjacency with new IDs (only forward edges)
    vector<unordered_set<int>> forward(n);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            int nu = new_id[u], nv = new_id[v];
            if (nu < nv) forward[nu].insert(nv);
        }
    }

    // Triangle counting
    int triangleCount = 0;
    for (int u = 0; u < n; ++u) {
        for (int v : forward[u]) {
            for (int w : forward[v]) {
                if (forward[u].count(w)) triangleCount++;
            }
        }
    }

    return triangleCount;
}
/*
int main(int argc, char* argv[]) {
    if (argc != 2) {
        cerr << "Usage: ./triangle_counter <filename.edge>\n";
        return 1;
    }

    ifstream infile(argv[1]);
    if (!infile) {
        cerr << "Error opening file.\n";
        return 1;
    }

    vector<pair<int, int>> edges;
    int max_node = -1;
    string line;

    while (getline(infile, line)) {
        if (line.empty() || line[0] == '#') continue;  // Ignore comments
        stringstream ss(line);
        int u, v;
        ss >> u >> v;
        if (u == v) continue; // Ignore self-loops
        edges.emplace_back(u, v);
        max_node = max({max_node, u, v});
    }

    infile.close();

    int n = max_node + 1;  // Total number of nodes
    int totalTriangles = countTriangles(n, edges);

    cout << "Total triangles: " << totalTriangles << endl;
    return 0;
}*/
