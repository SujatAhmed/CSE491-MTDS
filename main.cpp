
#include "preProcessing/include/adjacency.h"
#include "include/simulatedAnnealing.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

void printMap(map<int, vector<int>> adjacencyMap);
set<set<int>> readSeedTriangles(const string &filename);
set<set<int>> generateMaximalSubgraphs(set<set<int>> seedTriangles, float theta,
                                       map<int, vector<int>> adj);
void generatePredictedLabels(map<int, vector<int>> adjacencyMap,
                             set<set<int>> maximalSubgraphs,
                             string predictedFilePath);

int main(int argc, char *argv[]) {
    if (argc < 1) {
    cerr << "Usage: " << argv[0]
         << "<graph_filename>" << endl;
    return 1;
  }

  map<int, vector<int>> adjacencyMap;
  set<int> seed;
  float threshold = 0.5;
  map<int, vector<int>> graph;
  int temperature = 5;
  float alpha = 0.7;

  seed = {21,45,38};

  string base_dir = "/home/sujat/projects/cse491/graphs/";
  string file = argv[1];
  string filePath = base_dir + file;

  graph = generateAdjacencyMap(filePath);

  simulated_annealing_v(seed, threshold, graph, temperature, alpha);




}

set<set<int>> readSeedTriangles(const string &filename) {
  set<set<int>> seedTriangles;
  ifstream file(filename);
  string line;

  while (getline(file, line)) {
    istringstream iss(line);
    set<int> triangle;
    int node;

    while (iss >> node) {
      triangle.insert(node);
    }

    // Only accept triangle sets (size must be 3)
    if (triangle.size() == 3) {
      seedTriangles.insert(triangle);
    }
  }

  file.close();
  return seedTriangles;
}



void generatePredictedLabels(map<int, vector<int>> adjacencyMap,
                             set<set<int>> maximalSubgraphs,
                             string predictedFilePath) {
  // Step 1: Initialize all nodes with label -1
  map<int, int> labels;
  for (const auto &[node, _] : adjacencyMap) {
    labels[node] = -1;
  }

  // Step 2: Assign cluster IDs to nodes in subgraphs
  int clusterId = 1;
  for (const auto &subgraph : maximalSubgraphs) {
    for (int node : subgraph) {
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

  for (const auto &[node, label] : labels) {
    outfile << node << " " << label << "\n";
  }

  outfile.close();

  cout << "Predicted labels written to " << predictedFilePath << endl;
}
