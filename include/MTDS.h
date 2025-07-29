// MTDS.h
#ifndef MTDS_H
#define MTDS_H

#include "../include/subgraphAdjacency.h"
#include "../include/triangleDensity.h"
#include <set>           // for std::set
#include <unordered_set> // for std::unordered_set
#include <vector>        // for std::vector
#include <map>



// Given:
// - `adj`: full graph adjacency list
// - `graphSize`: number of vertices in the full graph
// - `seed`: initial vertex set
// - `theta`: triangle density threshold
// Returns:
// - A locally optimal triangle-dense subgraph
set<int> locally_optimal_triangle_dense_subgraph(
    map<int, vector<int>> &adj,
    set<int> &seed,
    float &theta
);

#endif // MTDS_H

