#include "include/triangleEnumeration.h"
#include "preProcessing/include/adjacency.h"
#include <vector>
using namespace std;
int main() { 
	vector<vector<int>> *adjacency = new vector<vector<int>>(); 
	string filename = "/home/sujat/projects/cse491/graphs/graph1.edges";
	build_adjacency_list(filename, adjacency);
}
