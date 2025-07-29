
#include "include/MTDS.h"
#include "preProcessing/include/adjacency.h"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
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

  if (argc < 4) {
    cerr << "Usage: " << argv[0]
         << " <graph_filename> <seed_filename> <predicted_filename>" << endl;
    return 1;
  }

  map<int, vector<int>> adjacencyMap;
  map<int, vector<int>> subgraphAdjacencyMap;
  set<set<int>> seedTriangles;
  set<set<int>> maximalSubgraphs;

  float theta = 0.2;

  string base_dir = "/home/sujat/projects/cse491/graphs/";
  string file = argv[1];
  string seedFileName = argv[2];
  string predictedFileName = argv[3];

  string filePath = base_dir + file;
  string seedPath = base_dir + seedFileName;
  string predictedPath = base_dir + predictedFileName;

  adjacencyMap = generateAdjacencyMap(filePath);
  seedTriangles = readSeedTriangles(seedPath);
  maximalSubgraphs =
      generateMaximalSubgraphs(seedTriangles, theta, adjacencyMap);
  generatePredictedLabels(adjacencyMap, maximalSubgraphs, predictedPath);
  // printMap(adjacencyMap);
  return 0;
}

void printMap(map<int, vector<int>> adjacencyMap) {

  for (const auto &[node, neighbors] : adjacencyMap) {
    cout << node << ": ";
    for (int neighbor : neighbors) {
      cout << neighbor << " ";
    }
    cout << endl;
  }
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

set<set<int>> generateMaximalSubgraphs(set<set<int>> seedTriangles, float theta,
                                       map<int, vector<int>> adj) {
  set<set<int>> maximalSubgraphs;

  for (const auto &seed : seedTriangles) {
    // Make a copy since the function takes a non-const reference
    set<int> seedCopy = seed;
    set<int> subgraph =
        locally_optimal_triangle_dense_subgraph(adj, seedCopy, theta);

    if (!subgraph.empty()) {
      maximalSubgraphs.insert(subgraph);
    }
  }

  return maximalSubgraphs;
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
