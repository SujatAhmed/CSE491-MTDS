// simulatedAnnealing.cpp
#include "../include/simulatedAnnealing.h"
#include "../include/subgraphAdjacency.h"
#include "../include/triangleEnumeration.h"
#include "../include/norm.h"
#include <cmath> // for exp()
#include <cstdlib>
#include <iostream>
#include <map>
#include <set>    // for std::set
#include <vector> // for std::vector
using namespace std;

// ANSI color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

set<int> find_neighbors(set<int> &subgraph, map<int, vector<int>> &graph);
float objective_function(int triangle_count, int subgraph_size);
bool should_accept(float delta, int temperature);
float sigmoid1(float x);

using namespace std;

set<int> simulated_annealing_v(set<int> &seed, float threshold,
                               map<int, vector<int>> &graph, int temperature,
                               float alpha, float norm_k) {

  set<int> S = seed;
  int triangle_count;
  int T_S = 0;
  int T_Sprime = 0;
  float E_S;
  float E_Sprime;
  set<int> neighbors;
  set<int> S_test;
  map<int, vector<int>> S_testMap;
  map<int, vector<int>> S_map;

  // cout << BOLD << YELLOW << "Initial seed:" << RESET;

  // for (int v : S)
  //   cout << " " << v;
  // cout << endl;

  int iter = 0;
  do {

    // cout << BOLD << CYAN << "\nIteration " << iter
    //      << " | Temperature: " << temperature << RESET << endl;

    neighbors = find_neighbors(S, graph);
    // cout << "Candidate neighbors:";
    // for (int u : neighbors)
    //   cout << " " << u;
    // cout << endl;

    for (int u : neighbors) {

      S_test = S;
      S_test.insert(u);
      S_testMap = generateSubgraphAdjacencyMap(graph, S_test);

      S_map = generateSubgraphAdjacencyMap(graph, S);

      T_Sprime = bruteForceTriangleCounting(S_testMap);
      T_S = bruteForceTriangleCounting(S_map);

      // E_S = objective_function(T_S, S.size());
      E_S = norm(T_S, S.size(), norm_k);
      // E_Sprime = objective_function(T_Sprime, S.size() + 1);
      E_Sprime = norm(T_Sprime, (S.size() + 1), norm_k);

      // cout << "Trying node " << u << ":";
      // cout << " Triangles if added = " << T_Sprime
      //      << " | Objective = " << E_Sprime << endl;
      // cout << " Current: Triangles = " << T_S << " | Objective = " << E_S
      //      << endl;

      if (E_Sprime >= E_S && E_Sprime >= threshold) {
        // cout << "Accepted (improvement or equal)" << endl;
        S.insert(u);
        triangle_count = T_Sprime;
      } else {
        float delta = E_Sprime - E_S;
        float prob = exp((delta * S.size()) / temperature);
        // cout << "Not improved. Delta = " << delta
        //      << ", Acceptance probability = " << prob << endl;
        if (should_accept(delta, temperature)) {
          // cout << "Accepted (probabilistic)" << endl;
          S.insert(u);
          triangle_count = T_Sprime;
        } else {
          // cout << "Rejected." << endl;
          triangle_count = T_S;
        }
      }
    }
    temperature *= alpha;
    ++iter;

  } while (temperature > 0);

  // cout << "\n" << BOLD << GREEN << "Final subgraph nodes:" << RESET;

  // for (int v : S)
  //   cout << " " << MAGENTA << v << RESET;

  // cout << endl;
  // cout << endl;
  // cout << "Final triangle count: " << triangle_count << endl;
  // cout << "Final temperature: " << temperature << endl;

  return S;
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

  return sigmoid1(triangleDensity);
}

float sigmoid1(float x) { return 1.0 / (1.0 + exp(-x)); }

bool should_accept(float delta, int temperature) {

  float prob = exp(delta / temperature);
  float r = static_cast<float>(rand()) / RAND_MAX;
  return r < prob;
}
