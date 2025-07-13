#include "preProcessing/include/adjacency.h"
#include "include/MTDS.h"
#include <vector>
#include <set>
using namespace std;

int main() {
  vector<vector<int>> *adjacency = new vector<vector<int>>();
  string filename = "/home/alek/CSE491-MTDS/graphs/graph1.edges";
  build_adjacency_list(filename, adjacency);
	set<int> seed = {1, 2, 4};
	locally_optimal_triangle_dense_subgraph(*adjacency, 6, seed, 0.1);

}
