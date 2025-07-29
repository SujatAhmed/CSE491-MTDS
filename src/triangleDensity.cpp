#include "../include/triangleEnumeration.h"
#include <iostream>
#include <ostream>
#include <vector>
#include <cmath>
using namespace std;

float sigmoid(float x) {
    return 1.0 / (1.0 + exp(-x));
}

float triangleDensity(
    map<int, vector<int>> adjacencyMap,
    int graphSize
) {

  int triangleCount = bruteForceTriangleCounting(adjacencyMap);

  float node_num = graphSize;

  float density = triangleCount / node_num;


  // float normalized_density = normalize(density);
  return sigmoid(density);
}


