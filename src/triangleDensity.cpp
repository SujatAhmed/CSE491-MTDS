#include "../include/triangleEnumeration.h"
#include <iostream>
#include <ostream>
#include <vector>
using namespace std;

float triangleDensity(
    map<int, vector<int>> adjacencyMap,
    int graphSize
) {

  int triangleCount = bruteForceTriangleCounting(adjacencyMap);

  float node_num = graphSize;

  float density = triangleCount / node_num;

  // float normalized_density = normalize(density);
  return density;
}
