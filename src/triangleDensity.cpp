#include "../include/triangleEnumeration.h"
#include <vector>
using namespace std;

float triangleDensity(int n, vector<vector<int>> adj, int size) { 
  int triangleCount = forward_triangle_listing(n, adj);
  float density = triangleCount/((size * (size - 1) * (size - 2)) / 6.0);
  return density;
}
