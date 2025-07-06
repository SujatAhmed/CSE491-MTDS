#ifndef TRIANGLE_ENUMERATION_H
#define TRIANGLE_ENUMERATION_H

#include <vector>
#include <utility> // for std::pair

using AdjacencyList = std::vector<std::vector<int>>;

bool degree_cmp(const std::pair<int, int>& a, const std::pair<int, int>& b);

void forward_triangle_listing(int n, const AdjacencyList& adjacency);

#endif // TRIANGLE_ENUMERATION_H
