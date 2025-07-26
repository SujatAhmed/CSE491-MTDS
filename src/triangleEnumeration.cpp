
#include <algorithm>
#include <iostream>
#include <unordered_set>
#include <vector>
#include "../include/triangleEnumeration.h"
using namespace std;

// Comparator to sort vertices by descending degree
bool degree_cmp(const pair<int, int> &a, const pair<int, int> &b) {
  return a.second > b.second;
}

// Triangle listing using the forward algorithm
int forward_triangle_listing(int n, const vector<vector<int>> &adjacency) {
  int count = 0;
  // Step 1: Sort vertices by descending degree
  vector<pair<int, int>> degrees;
  for (int i = 0; i < n; ++i)
    degrees.emplace_back(i, adjacency[i].size());

  sort(degrees.begin(), degrees.end(), degree_cmp);

  // Step 2: Order vertices and build rank mapping
  vector<int> rank(n);
  vector<vector<int>> ordered_adj(n);
  for (int i = 0; i < n; ++i)
    rank[degrees[i].first] = i;

  for (int u = 0; u < n; ++u) {
    for (int v : adjacency[u]) {
      if (rank[u] < rank[v])
        ordered_adj[rank[u]].push_back(rank[v]);
    }
  }

  // Step 3: Forward algorithm
  vector<unordered_set<int>> A(n); // A(t)
  for (int s = 0; s < n; ++s) {
    for (int t : ordered_adj[s]) {
      for (int v : A[s]) {
        if (A[t].find(v) != A[t].end()) {
          // Output triangle (v, s, t) in original vertex ids
          int orig_v = degrees[v].first;
          int orig_s = degrees[s].first;
          int orig_t = degrees[t].first;
          // cout << "Triangle: " << orig_v << " " << orig_s << " " << orig_t
               // << "\n";
          count ++;
        }
      }
      A[t].insert(s);
    }
  }
  cout << "Triangle Count = " << count << endl;
  return count;
}