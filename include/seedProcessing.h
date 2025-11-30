#ifndef SEED_PROCESSING_H
#define SEED_PROCESSING_H

#include <map>
#include <set>
#include <string>
#include <vector>

struct SeedMetrics {
  std::set<int> nodes;
  int triangle_count;
  float density;
};

std::vector<SeedMetrics> read_seeds_with_density(
    const std::string &filename,
    const std::map<int, std::vector<int>> &graph,
    float theta,
    float k);

#endif // SEED_PROCESSING_H
