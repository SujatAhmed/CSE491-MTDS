
#include "../include/subgraphAdjacency.h"
#include "../include/triangleDensity.h"
#include <iostream>
#include <set>           // for std::set
#include <unordered_set> // for std::unordered_set
#include <vector>        // for std::vector
using namespace std;

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

    // Step 1: Try adding vertices
    unordered_set<int> candidate_add;
    for (int u : Vt) {
      for (int v : adj[u]) {
        if (Vt.count(v) == 0)
          candidate_add.insert(v);
      }
    }

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
