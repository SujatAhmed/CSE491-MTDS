#ifndef TRIANGLE_ENUMERATION_H
#define TRIANGLE_ENUMERATION_H

#include <map>
#include <utility>
#include <vector>
using namespace std;

int bruteForceTriangleCounting(map<int, vector<int>> adjacencyMap);
int countTrianglesForNode(const map<int, vector<int>> &subgraphAdjacency,
                          int node);

#endif // TRIANGLE_ENUMERATION_H
