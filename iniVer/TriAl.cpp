// TriAl.cpp
#include "TriAl.h"
#include <vector>
#include <unordered_set>
#include <algorithm>

using namespace std;

bool degree_cmp(const pair<int, int>& a, const pair<int, int>& b) {
    return (a.second < b.second) || (a.second == b.second && a.first < b.first);
}

int countTriangles(int n, const vector<pair<int, int>>& edges) {
    vector<vector<int>> adj(n);
    vector<int> degree(n, 0);

    for (auto [u, v] : edges) {
        adj[u].push_back(v);
        adj[v].push_back(u);
        degree[u]++;
        degree[v]++;
    }

    vector<int> new_id(n);
    vector<pair<int, int>> nodes(n);
    for (int i = 0; i < n; ++i) nodes[i] = {i, degree[i]};
    sort(nodes.begin(), nodes.end(), degree_cmp);
    for (int i = 0; i < n; ++i) new_id[nodes[i].first] = i;

    vector<unordered_set<int>> forward(n);
    for (int u = 0; u < n; ++u) {
        for (int v : adj[u]) {
            int nu = new_id[u], nv = new_id[v];
            if (nu < nv) forward[nu].insert(nv);
        }
    }

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
