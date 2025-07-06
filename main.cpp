#include "preProcessing/include/adjacency.h"
#include "include/MTDS.h"
#include <vector>
#include <set>
using namespace std;

int main() {
  vector<vector<int>> *adjacency = new vector<vector<int>>();
  string filename = "/home/sujat/projects/cse491/graphs/graph1.txt";
  build_adjacency_list(filename, adjacency);
	set<int> seed = {1, 2, 4};
	locally_optimal_triangle_dense_subgraph(*adjacency, 6, seed, 0.1);

}
