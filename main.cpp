#include "include/MTDS.h"
#include "preProcessing/include/adjacency.h"
#include <set>
#include <vector>
using namespace std;

int main() {
  vector<vector<int>> *adjacency = new vector<vector<int>>();
  string filename =
      "/home/sujat/projects/cse491/graphs/small_world_graph_m_5_10.txt";
  build_adjacency_list(filename, adjacency);
  // 8,3,2 for the 10 node graph 
  set<int> seed = {8,2,4};
  locally_optimal_triangle_dense_subgraph(*adjacency, adjacency->size(), seed, 0.3);
}
