#include "../include/subgraphAdjacency.h"
#include "../include/triangleDensity.h"
#include <iostream>
#include <set>           // for std::set
#include <unordered_set> // for std::unordered_set
#include <vector>        // for std::vector
using namespace std;

set<int> cluster_one_version(map<int, vector<int>> &adj,
                                                 set<int> &seed, float &theta) {
  set<int> Vt = seed;
  int t = 0;
  float f_Vprime;

  while (true) {
    set<int> Vt1 = Vt;
    float f_Vt = 0; // need to find the triangle density of vt1

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
      map<int, vector<int>> subgraphAdjacencyMap =
          generateSubgraphAdjacencyMap(adj, V_prime);

      f_Vprime = triangleDensity(subgraphAdjacencyMap, V_prime.size());

      cout << "Triangle Density After Addition of Node: " << v << ": " << f_Vprime << endl;
      if (f_Vprime >= theta && f_Vprime >= f_Vt) {
        Vt1 = V_prime;
        f_Vt = f_Vprime;
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
    cout << endl;

    for (int v : candidate_remove) {
      // set<int> V_doubleprime = Vt1;
      map<int, vector<int>> subgraphAdjacencyMap =
          generateSubgraphAdjacencyMap(adj, Vt1);
      int triangleCount = countTrianglesForNode(subgraphAdjacencyMap, v);
      if(triangleCount == 0){
        cout << "triangle participation of " << v << " is " << triangleCount << " ";
        cout << "removing node: " << v << endl;
        Vt1.erase(v);
      }
    }

    cout << endl;

    cout << "Subgraph After Iteration: " << t << ": " << endl;
    for (int val : Vt1){
      cout << val << " ";
    }
    cout << endl;
    cout << "-------------------------------" << endl;
    cout << endl;

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
  cout <<  "=================================" << endl;

  return Vt;
}
