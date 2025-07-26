#include "../include/triangleEnumeration.h"
#include <vector>
#include <iostream>
using namespace std;

float triangleDensity(int n, vector<vector<int>> adj, int size) { 
  int triangleCount = forward_triangle_listing(n, adj);
  float possibleTriangles = (float)((size * (size - 1) * (size - 2)) / 6.0);
  float density = (float)triangleCount / possibleTriangles;
  cout << "\n \n denity: " <<  density << " \n \n ";
  return density;
}
