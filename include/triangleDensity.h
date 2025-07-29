
// triangleDensity.h
#ifndef TRIANGLE_DENSITY_H
#define TRIANGLE_DENSITY_H

#include <vector>
#include <map>
#include "triangleEnumeration.h"
using  namespace std;

// Forward declaration of the triangle listing function.
// Ensure this matches the signature in triangleEnumeration.h

// Computes the triangle density of a graph with `n` vertices and an adjacency list `adj`.
// `size` is the number of vertices in the (sub)graph.
float triangleDensity(
        map<int, vector<int>> adjacencyMap,
        int graphSize
);

#endif // TRIANGLE_DENSITY_H
