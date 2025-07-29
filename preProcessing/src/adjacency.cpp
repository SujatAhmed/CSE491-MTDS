
#include "../include/adjacency.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

map<int, vector<int>> generateAdjacencyMap(const string &filename) {
  map<int, vector<int>> graph;
  ifstream infile(filename);

  if (!infile) {
    cerr << "Error: Cannot open file " << filename << endl;
    return graph; // return empty map
  }

  int u, v;
  while (infile >> u >> v) {
    // Avoid duplicate entries
    if (find(graph[u].begin(), graph[u].end(), v) == graph[u].end())
      graph[u].push_back(v);
    if (find(graph[v].begin(), graph[v].end(), u) == graph[v].end())
      graph[v].push_back(u);
  }

  infile.close();
  return graph;
}
