#ifndef SIMULATED_ANNEALING_V_H
#define SIMULATED_ANNEALING_V_H

#include <set>
#include <map>
#include <vector>

// Finds neighbors of the current subgraph in the full graph.
// Returns a set of node ids not in subgraph but adjacent to at least one subgraph node.
std::set<int> find_neighbors(std::set<int> &subgraph, std::map<int, std::vector<int>> &graph);

// Objective function for subgraph quality, based on triangle count and subgraph size.
float objective_function(int triangle_count, int subgraph_size);

// Sigmoid function for normalization.
float sigmoid1(float x);

// Determines whether to accept a move in simulated annealing, given delta and temperature.
bool should_accept(float delta, int temperature);

// Simulated annealing algorithm to find a triangle-dense subgraph.
// Parameters:
//   seed - Initial node set
//   threshold - Triangle density threshold (not currently used in logic, but for reference)
//   graph - Full graph adjacency map
//   temperature - Initial temperature
//   alpha - Cooling rate
// Returns:
//   Final node set of the subgraph
std::set<int> simulated_annealing_v(
    std::set<int> &seed,
    float threshold,
    std::map<int, std::vector<int>> &graph,
    int temperature,
    float alpha
);

#endif // SIMULATED_ANNEALING_V_H
