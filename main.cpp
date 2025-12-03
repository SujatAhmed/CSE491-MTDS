#include "include/seedProcessing.h"
#include "include/simulatedAnnealing.h"
#include "preProcessing/include/adjacency.h"
#include "include/subgraphAdjacency.h"
#include "include/triangleEnumeration.h"
#include "include/norm.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
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

void generatePredictedLabels(const map<int, vector<int>>& adjacencyMap,
                             const vector<set<int>>& maximalSubgraphs,
                             const string& predictedFilePath) {
  // Step 1: Initialize all nodes with label -1
  map<int, int> labels;
  for (const auto &[node, _] : adjacencyMap) {
    labels[node] = -1;
  }

  // Step 2: Assign cluster IDs to nodes in subgraphs
  // Cluster IDs start at 1.
  int clusterId = 1;
  for (const auto &subgraph : maximalSubgraphs) {
    for (int node : subgraph) {
      // Note: If a node appears in multiple maximal subgraphs, it will be 
      // labeled with the ID of the last subgraph it appears in.
      labels[node] = clusterId;
    }
    clusterId++;
  }

  // Step 3: Write labels to file
  ofstream outfile(predictedFilePath);
  if (!outfile.is_open()) {
    cerr << "Error opening file: " << predictedFilePath << endl;
    return;
  }

  // Write node IDs and their assigned labels
  for (const auto &[node, label] : labels) {
    outfile << node << " " << label << "\n";
  }

  outfile.close();

}

int main(int argc, char *argv[]) {

  if (argc < 7) {
    cerr << "Usage: " << argv[0]
         << " graph=<graph_filename> seed=<seed_filename>"
         << " density=<float> temperature=<int> alpha=<float> normalize_const_k=<float>" << endl;
    return 1;
  }

  map<string, string> args;

  // Parse arguments as key=value
  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    size_t pos = arg.find('=');
    if (pos != string::npos) {
      string key = arg.substr(0, pos);
      string value = arg.substr(pos + 1);
      args[key] = value;
    }
  }

  // Validate required arguments
  string required[] = {"graph", "seed", "density", "temperature", "alpha"};
  for (auto &key : required) {
    if (args.find(key) == args.end()) {
      cerr << "Error: missing required argument '" << key << "'" << endl;
      return 1;
    }
  }

  // Extract arguments with correct types
  string graph_filename = args["graph"];
  string seed_filename = args["seed"];
  float density = stof(args["density"]);
  int temperature = stoi(args["temperature"]);
  float alpha = stof(args["alpha"]);
  float norm_k = stof(args["normalize_const_k"]);

  // string base_dir = "TestGraphs/Graphs/";
  // string filePath = base_dir + graph_filename;
  // string seed_path = base_dir + "seeds/" + seed_filename;

  string filePath = graph_filename;
  string seed_path = seed_filename;

  map<int, vector<int>> graph;
  vector<SeedMetrics> seeds;
  vector<set<int>> maximal_subgraphs;

  graph = generateAdjacencyMap(filePath);
  seeds = read_seeds_with_density(seed_path, graph, density, norm_k);

  // cout << "Seed path --> " << seed_path << endl;

  // //Test Start
  cout << "\n--- DEBUG: Seeds Loaded ---" << endl;
  cout << "Total seeds = " << seeds.size() << endl;

  for (size_t i = 0; i < seeds.size(); i++) {
      const auto& s = seeds[i];

      cout << "Seed " << i + 1 << ": { ";
      for (int n : s.nodes) cout << n << " ";
      cout << "}";

      cout << " | Triangles: " << s.triangle_count
          << " | Density: " << s.density << endl;
  }
  // // Test End

  for (const auto &seed_set : seeds) {
    // cout << "Seed:";
    // for (int node : seed_set.nodes) {
    //   cout << " " << node;
    // }
    // cout << " | Triangles: " << seed_set.triangle_count
    //      << " | Density: " << seed_set.density << endl;

    auto mutable_seed = seed_set.nodes;
    set<int> s_maximal_sg;
    s_maximal_sg = simulated_annealing_v(mutable_seed, density, graph, temperature, alpha, norm_k);
    maximal_subgraphs.push_back(s_maximal_sg);
  }

  // cout << "\n" << BOLD << YELLOW << "--- Maximal Subgraphs ---" 
    //  << RESET << endl;

  int cid = 1;
  for (const auto& sg : maximal_subgraphs) {
      int numNode = sg.size();
      map<int, vector<int>> numTriSub = generateSubgraphAdjacencyMap(graph, sg);
      int numTri = bruteForceTriangleCounting(numTriSub);
      float triangleDensity = norm(numTri, numNode, norm_k);


      cout << "\nSubgraph " << cid << ":z { ";
      for (int n : sg) cout << n << " ";
      cout << "} N: " << numNode << " Triangles: " << numTri << " Density: " << triangleDensity;
      cid++;
  }
  cout << endl;

  // Construct a descriptive output filename
  string output_basename = graph_filename;
  // Remove file extension if present (e.g., '.txt', '.gml')
  size_t lastdot = output_basename.find_last_of(".");
  if (lastdot != string::npos) {
    output_basename = output_basename.substr(0, lastdot);
  }
  size_t lastslash = output_basename.find_last_of("/\\");
  if (lastslash != string::npos) {
      output_basename = output_basename.substr(lastslash + 1);
  }
  
  string predictedFilePath = "/home/alek/CSE491-MTDS/TestGraphs/Graphs/PredictedLabels/" + output_basename + "_predicted_labels.txt";

  generatePredictedLabels(graph, maximal_subgraphs, predictedFilePath);

  return 0;

}