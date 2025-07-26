// TriAl.h
#ifndef TRIAL_H
#define TRIAL_H

#include <vector>
#include <utility>

bool degree_cmp(const std::pair<int, int>& a, const std::pair<int, int>& b);
int countTriangles(int n, const std::vector<std::pair<int, int>>& edges);

#endif
