#include "../include/incrementalTriangleCount.h"
#include <cmath> // for exp()
#include <cstdlib>
#include <map>
#include <set>    // for std::set
#include <vector> // for std::vector

set<int> find_neighbors(set<int> &subgraph, map<int, vector<int>> &graph);
float objective_function(int triangle_count, int subgraph_size);
bool should_accept(float delta, int temperature);
float sigmoid(float x);

using namespace std;

map<int, vector<int>> simulated_annealing_v(set<int> &seed, float threshold,
                                            map<int, vector<int>> &graph,
                                            int temperature, float alpha) {

  set<int> S = seed;
  int triangle_count = 1;
  int T_S = 0;
  int T_Sprime = 0;
  float E_S;
  float E_Sprime;
  set<int> neighbors;

  do {
    neighbors = find_neighbors(S, graph);
    for (int u : neighbors) {

      T_Sprime = triangle_count + incremental_triangle_count(S, graph, u);
      T_S = triangle_count;

      E_S = objective_function(T_S, S.size());
      E_Sprime = objective_function(T_Sprime, (S.size() + 1));

      if (E_Sprime >= E_S) {
        S.insert(u);
        triangle_count = T_Sprime;
      }

      else {
        float delta = E_Sprime - E_S;
        if (should_accept(delta, temperature)) {
          S.insert(u);
          triangle_count = T_Sprime;
        }
        triangle_count = T_S;
      }
    }
    temperature *= alpha;

  } while (temperature > 0);

  return graph;
}

set<int> find_neighbors(set<int> &subgraph, map<int, vector<int>> &graph) {

  set<int> neighbors;
  for (int u : subgraph) {
    auto it = graph.find(u);
    if (it != graph.end()) {
      for (int v : it->second) {
        if (!subgraph.count(v)) {
          neighbors.insert(v);
        }
      }
    }
  }
  return neighbors;
}

float objective_function(int triangle_count, int subgraph_size) {

  float triangleDensity = static_cast<float>(triangle_count) / subgraph_size;

  return sigmoid(triangleDensity);
}

float sigmoid(float x) { return 1.0 / (1.0 + exp(-x)); }

bool should_accept(float delta, int temperature) {

  float prob = exp(delta / temperature);
  float r = static_cast<float>(rand()) / RAND_MAX;
  return r < prob;
}
