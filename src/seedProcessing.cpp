#include "../include/seedProcessing.h"
#include "../include/norm.h"
#include "../include/subgraphAdjacency.h"
#include "../include/triangleEnumeration.h"

#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace {
string trim(const string &value) {
  const string whitespace = " \t\n\r";
  const auto start = value.find_first_not_of(whitespace);
  if (start == string::npos) {
    return "";
  }
  const auto end = value.find_last_not_of(whitespace);
  return value.substr(start, end - start + 1);
}
}

vector<SeedMetrics> read_seeds_with_density(const string &filename,
                                            const map<int, vector<int>> &graph,
                                            float theta, float k) {
  vector<SeedMetrics> filtered_seeds;
  ifstream fin(filename);

  if (!fin.is_open()) {
    cerr << "Failed to open seed file: " << filename << endl;
    return filtered_seeds;
  }

  string raw_seed;
  while (getline(fin, raw_seed, ',')) {
    string seed_text = trim(raw_seed);
    if (seed_text.empty()) {
      continue;
    }

    istringstream seed_stream(seed_text);
    set<int> seed_nodes;
    int node = 0;
    while (seed_stream >> node) {
      seed_nodes.insert(node);
    }

    if (seed_nodes.empty()) {
      continue;
    }

    int triangle_count = 0;
    float density = 0.0f;

    if (seed_nodes.size() >= 3) {
      map<int, vector<int>> subgraph =
          generateSubgraphAdjacencyMap(graph, seed_nodes);
      triangle_count = bruteForceTriangleCounting(subgraph);
      density = norm(triangle_count, static_cast<int>(seed_nodes.size()), k);
    }

    if (density >= theta) {
      filtered_seeds.push_back({seed_nodes, triangle_count, density});
    }
  }

  return filtered_seeds;
}
