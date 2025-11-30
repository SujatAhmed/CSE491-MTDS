
#include "include/seedProcessing.h"
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

struct ProgramOptions {
  string graph_filename;
  string seed_filename;
  float density;
  int temperature;
  float alpha;
};

struct DataPaths {
  string graph_path;
  string seed_path;
};

// ANSI color codes
const string RESET = "\033[0m";
const string RED = "\033[31m";
const string GREEN = "\033[32m";
const string YELLOW = "\033[33m";
const string BLUE = "\033[34m";
const string MAGENTA = "\033[35m";
const string CYAN = "\033[36m";
const string BOLD = "\033[1m";

ProgramOptions parseProgramOptions(int argc, char *argv[]);
DataPaths buildDataPaths(const ProgramOptions &options);
void printProgramOptions(const ProgramOptions &options);
set<set<int>> processSeeds(const vector<SeedMetrics> &seeds, const map<int, vector<int>> &graph,
                           const ProgramOptions &options);

int main(int argc, char *argv[]) {
  if (argc < 6) {
    cerr << "Usage: " << argv[0]
         << " graph=<graph_filename> seed=<seed_filename> density=<float>"
         << " temperature=<int> alpha=<float>" << endl;
    return 1;
  }

  ProgramOptions options = parseProgramOptions(argc, argv);
  DataPaths paths = buildDataPaths(options);

  printProgramOptions(options);
  cout << "Seed path: " << paths.seed_path << endl;

  map<int, vector<int>> graph = generateAdjacencyMap(paths.graph_path);
  vector<SeedMetrics> seeds = read_seeds_with_density(paths.seed_path, graph, options.density, 0.0001f);

  set<set<int>> maximal_subgraphs = processSeeds(seeds, graph, options);
  (void)maximal_subgraphs;

  return 0;
}

ProgramOptions parseProgramOptions(int argc, char *argv[]) {
  map<string, string> args;

  for (int i = 1; i < argc; ++i) {
    string arg = argv[i];
    size_t pos = arg.find('=');
    if (pos != string::npos) {
      string key = arg.substr(0, pos);
      string value = arg.substr(pos + 1);
      args[key] = value;
    }
  }

  string required[] = {"graph", "seed", "density", "temperature", "alpha"};
  for (auto &key : required) {
    if (args.find(key) == args.end()) {
      cerr << "Error: missing required argument '" << key << "'" << endl;
      exit(1);
    }
  }

  ProgramOptions options{};
  options.graph_filename = args["graph"];
  options.seed_filename = args["seed"];
  options.density = stof(args["density"]);
  options.temperature = stoi(args["temperature"]);
  options.alpha = stof(args["alpha"]);

  return options;
}

DataPaths buildDataPaths(const ProgramOptions &options) {
  const string base_dir = "/home/sujat/projects/cse491/TestGraphs/Graphs/";
  DataPaths paths{};
  paths.graph_path = base_dir + options.graph_filename;
  paths.seed_path = base_dir + "seeds/" + options.seed_filename;
  return paths;
}

void printProgramOptions(const ProgramOptions &options) {
  cout << BOLD << GREEN << "Graph filename: " << RESET << BLUE << options.graph_filename
       << RESET << endl;
  cout << BOLD << GREEN << "Seed filename: " << RESET << BLUE << options.seed_filename
       << RESET << endl;
  cout << BOLD << GREEN << "Density: " << RESET << YELLOW << options.density << RESET
       << endl;
  cout << BOLD << GREEN << "Temperature: " << RESET << MAGENTA << options.temperature << RESET
       << endl;
  cout << BOLD << GREEN << "Alpha: " << RESET << CYAN << options.alpha << RESET << endl;
}

set<set<int>> processSeeds(const vector<SeedMetrics> &seeds, const map<int, vector<int>> &graph,
                           const ProgramOptions &options) {
  set<set<int>> maximal_subgraphs;

  for (const auto &seed_set : seeds) {
    cout << "Seed:";
    for (int node : seed_set.nodes) {
      cout << " " << node;
    }
    cout << " | Triangles: " << seed_set.triangle_count << " | Density: " << seed_set.density
         << endl;

    auto mutable_seed = seed_set.nodes;
    set<int> maximal = simulated_annealing_v(mutable_seed, options.density, graph,
                                             options.temperature, options.alpha);
    maximal_subgraphs.insert(maximal);
  }

  return maximal_subgraphs;
}
