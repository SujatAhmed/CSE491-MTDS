#include "../include/subgraphAdjacency.h"
#include "../include/triangleDensity.h"
#include <iostream>
#include <set>           // for std::set
#include <unordered_set> // for std::unordered_set
#include <vector>        // for std::vector
using namespace std;

double triangleDensity(const vector<vector<int>> &adj,
                       const vector<int> &subset) {
  int num_triangles = 0;
  int k = subset.size(); // Number of nodes in the subgraph

  // If there are fewer than 3 nodes, no triangles are possible.
  if (k < 3) {
    return 0.0f;
  }

  // Iterate through all unique combinations of three nodes (u, v, w) from the
  // subset
  for (int i = 0; i < k; ++i) {
    for (int j = i + 1; j < k; ++j) {
      for (int l = j + 1; l < k; ++l) {
        int u = subset[i];
        int v = subset[j];
        int w = subset[l];

        // Check if edges (u,v), (v,w), and (w,u) exist
        // The adj matrix stores 1 for an edge, 0 otherwise.
        if (adj[u][v] == 1 && adj[v][w] == 1 && adj[w][u] == 1) {
          num_triangles++;
        }
      }
    }
  }

  // Calculate the number of possible triangles (k choose 3)
  // Formula: k * (k - 1) * (k - 2) / 6
  // Use long long for intermediate calculation to prevent overflow before
  // division
  int possible_triangles = (int)k * (k - 1) * (k - 2) / 6;

  if (possible_triangles == 0) {
    return 0.0f; // Avoid division by zero if k < 3 (already handled, but good
                 // for safety)
  }

  // cout << "\n\n Tri: " << num_triangles << "\n pt: " << possible_triangles <<
  // "\n\n";

  return static_cast<double>(num_triangles) / possible_triangles;
}

set<int> locally_optimal_triangle_dense_subgraph(const vector<vector<int>> &adj,
                                                 int graphSize,
                                                 const set<int> &seed,
                                                 double theta) {
  set<int> Vt = seed;
  int t = 0;
  double f_Vprime;

  while (true) {
    set<int> Vt1 = Vt;
    double f_Vt = 0; // need to find the triangle density of vt1
    
    cout << "IN WHILE LOOP" << endl;

    // Step 1: Try adding vertices
    unordered_set<int> candidate_add;
    for (int u : Vt) {
      cout << "In the candidate add loop" << endl;
      for (int v : adj[u]) {
        cout << "v: " << v << " in u: " << u << endl;
        if (Vt.count(v) == 0)
          candidate_add.insert(v);
      }
    }
    cout << "After adding vertices" << endl; 

    for (int v : candidate_add) {
      set<int> V_prime = Vt1;

      V_prime.insert(v);
      vector<vector<int>> subgraphAdjacency =
          get_subgraph_adjacency(adj, V_prime);

      f_Vprime = triangleDensity(graphSize, subgraphAdjacency, V_prime.size());

      cout << "Triangle Density After Addition: " << f_Vprime << endl;
      if (f_Vprime >= theta) {
        Vt1 = V_prime;
        // f_Vt = f_Vprime;
      }
    }

    // Step 2: Try removing vertices
    unordered_set<int> candidate_remove;
    for (int u : Vt1) {
      for (int v : adj[u]) {
        if (Vt1.count(v) == 0)
          candidate_remove.insert(u);
      }
    }

    for (int v : candidate_remove) {
      set<int> V_doubleprime = Vt1;
      V_doubleprime.erase(v);
      vector<vector<int>> subgraphAdjacency =
          get_subgraph_adjacency(adj, V_doubleprime);

      double f_Vdoubleprime =
          triangleDensity(graphSize, subgraphAdjacency, V_doubleprime.size());
      cout << "Triangle Density After Deletion: " << f_Vdoubleprime << endl;

      if (f_Vdoubleprime >= theta) {
        Vt1 = V_doubleprime;
        f_Vt = f_Vdoubleprime;
      }
    }

    if (Vt == Vt1)
      break;

    Vt = Vt1;
    ++t;
  }
  cout << "Maximal Subgraph" << endl;
  for (int val : Vt) {
    cout << val << " ";
  }
  cout << endl;

  return Vt;
}
