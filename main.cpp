
#include "include/MTDS.h"
#include "include/subgraphAdjacency.h"
#include "include/triangleEnumeration.h"
#include "preProcessing/include/adjacency.h"
#include <iostream>
#include <set>
#include <vector>
using namespace std;

int main(int argc, char *argv[]) {

  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <graph_filename>" << endl;
    return 1;
  }

  string filename = argv[1];

  vector<vector<int>> *adjacency = new vector<vector<int>>();
  build_adjacency_list(filename, adjacency);
  // forward_triangle_listing(adjacency->size(), *adjacency);

  // vector<vector<int>> sub_adj = get_subgraph_adjacency(*adjacency,
  // {39,27,2,35}); for (const auto &row : sub_adj) {
  //   for (int val : row) {
  //     cout << val << " ";
  //   }
  //   cout << endl;
  // }
  // forward_triangle_listing(sub_adj.size(), sub_adj);

  set<int> mySet = {39, 27, 2};
  locally_optimal_triangle_dense_subgraph(*adjacency, adjacency->size(),
                                          mySet, 0.3);

  cout << adjacency->size() << endl;

  delete adjacency; // Clean up the allocated memory
  return 0;
}
