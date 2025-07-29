
#include "include/MTDS.h"
#include "include/subgraphAdjacency.h"
#include "include/triangleEnumeration.h"
#include "preProcessing/include/adjacency.h"
#include <iostream>
#include <map>
#include <set>
#include <vector>
using namespace std;

void printMap(map<int, vector<int>> adjacencyMap);

int main(int argc, char *argv[]) {

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <graph_filename>" << endl;
    return 1;
  }
  map<int, vector<int>> adjacencyMap;
  map<int, vector<int>> subgraphAdjacencyMap;
  set<int> subgraph = {39,27,35,2};

  string filename = argv[1];

  adjacencyMap = generateAdjacencyMap(filename);
  subgraphAdjacencyMap = generateSubgraphAdjacencyMap(adjacencyMap,subgraph);
  int count = bruteForceTriangleCounting(subgraphAdjacencyMap);

  cout << "triangle count: " << count << endl;

  // printMap(adjacencyMap);
  // printMap(subgraphAdjacencyMap);

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



