
#include "include/simulatedAnnealing.h"
#include "preProcessing/include/adjacency.h"
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

set<set<int>> read_seed_file(const string &filename);
set<set<int>> generateMaximalSubgraphs(set<set<int>> seedTriangles, float theta,
                                       map<int, vector<int>> adj);

int main(int argc, char *argv[]) {

  if (argc < 6) {
    cerr << "Usage: " << argv[0]
         << " graph=<graph_filename> seed=<seed_filename>"
         << " density=<float> temperature=<int> alpha=<float>" << endl;
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

  // Example output
  cout << BOLD << GREEN << "Graph filename: " << RESET << BLUE << graph_filename
       << RESET << endl;

  cout << BOLD << GREEN << "Seed filename: " << RESET << BLUE << seed_filename
       << RESET << endl;

  cout << BOLD << GREEN << "Density: " << RESET << YELLOW << density << RESET
       << endl;

  cout << BOLD << GREEN << "Temperature: " << RESET << MAGENTA << temperature
       << RESET << endl;

  cout << BOLD << GREEN << "Alpha: " << RESET << CYAN << alpha << RESET << endl;

  string base_dir = "/home/sujat/projects/cse491/graphs/";
  string filePath = base_dir + graph_filename;
  string seed_path = base_dir + "seeds/" + seed_filename;

  map<int, vector<int>> adjacencyMap;
  map<int, vector<int>> graph;
  set<set<int>> seeds;

  seeds = read_seed_file(seed_path);
  graph = generateAdjacencyMap(filePath);

  cout << "seed path " << seed_path << endl;

  for (const auto &seed_set : seeds) {
    auto mutable_seed = seed_set;
    simulated_annealing_v(mutable_seed, density, graph, temperature, alpha);
  }

  // for (const auto &seed_set : seeds) {
  //   for (int node : seed_set) {
  //     cout << node << " ";
  //   }
  //   cout << endl;
  // }
}

set<set<int>> read_seed_file(const string &filename) {
  set<set<int>> all_seeds;
  ifstream fin(filename);
  if (!fin.is_open()) {
    cerr << "Failed to open seed file: " << filename << endl;
    return all_seeds;
  }
  string line;
  while (getline(fin, line)) {
    istringstream iss(line);
    set<int> seed_set;
    int node;
    while (iss >> node) {
      seed_set.insert(node);
    }
    if (!seed_set.empty()) {
      all_seeds.insert(seed_set);
    }
  }
  return all_seeds;
}
