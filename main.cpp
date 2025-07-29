
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

  string filename = argv[1];

  adjacencyMap = generateAdjacencyMap(filename);
  printMap(adjacencyMap);

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
