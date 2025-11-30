# Using Seed Processing in `main.cpp`

The seed processing workflow is integrated into `main.cpp` to vet candidate seeds before simulated annealing.

## Argument handling
`main` expects a seed file path via `seed=<seed_filename>` alongside graph and annealing parameters. The program builds paths to the graph and seed files, then loads the graph adjacency map with `generateAdjacencyMap`.

## Loading and filtering seeds
`read_seeds_with_density` is called with the parsed seed file, the full graph adjacency map, the user-specified density threshold, and a small `k` value used by `norm`:

```cpp
seeds = read_seeds_with_density(seed_path, graph, density, 0.0001f);
```

This returns only seeds whose density meets or exceeds `density`, while also providing each seed's triangle count and computed density.

## Logging seed metrics
For each returned `SeedMetrics` entry, `main` prints the node list, triangle count, and density to standard output to make the filtering results visible:

```cpp
for (const auto &seed_set : seeds) {
  cout << "Seed:";
  for (int node : seed_set.nodes) {
    cout << " " << node;
  }
  cout << " | Triangles: " << seed_set.triangle_count
       << " | Density: " << seed_set.density << endl;
  // ...
}
```

## Feeding seeds to simulated annealing
Each seed's node set is then passed to `simulated_annealing_v`, along with the density threshold, graph, and annealing parameters. The resulting maximal subgraphs are collected into `maximal_subgraphs` for downstream use:

```cpp
auto mutable_seed = seed_set.nodes;
set<int> s_maximal_sg;
s_maximal_sg = simulated_annealing_v(mutable_seed, density, graph, temperature, alpha);
maximal_subgraphs.insert(s_maximal_sg);
```

This flow ensures that simulated annealing only processes seeds that already satisfy the desired density criterion and leverages the precomputed triangle counts and densities for transparency.
