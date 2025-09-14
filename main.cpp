
#include "include/MTDS.h"
#include "preProcessing/include/adjacency.h"
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
