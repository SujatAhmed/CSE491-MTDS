# Seed Processing Module

The seed processing utility is implemented in `include/seedProcessing.h` with logic in `src/seedProcessing.cpp`. Its purpose is to parse seed definitions, compute their structural metrics, and keep only seeds that meet a density requirement.

## Key responsibilities
- **Parse comma-separated seeds**: The input seed file is read as comma-delimited segments. Each segment contains space-separated node identifiers that together form a seed subgraph.
- **Normalize seed text**: Extra whitespace around each comma-delimited entry is trimmed so that seeds can be read even if the file contains irregular spacing.
- **Build seed node sets**: Each seed is collected into a `std::set<int>` to deduplicate nodes while preserving deterministic ordering for later processing.
- **Compute triangle counts**: For seeds with at least three nodes, the module constructs a subgraph adjacency map using `generateSubgraphAdjacencyMap` and counts triangles via `bruteForceTriangleCounting`.
- **Calculate density**: The triangle count and seed size are combined with the configured `k` parameter through the existing `norm` function to produce a density score.
- **Filter by threshold**: Only seeds whose density is greater than or equal to the caller-provided `theta` are returned to the caller.

## Public API
### `std::vector<SeedMetrics> read_seeds_with_density(const std::string &filename, const std::map<int, std::vector<int>> &graph, float theta, float k)`
Reads all seeds from the given file, computes triangle count and density for each seed, and returns a vector of `SeedMetrics` containing only the seeds meeting the threshold.

**Parameters**
- `filename`: Path to the comma-delimited seed file.
- `graph`: Graph adjacency map used to build subgraphs for triangle enumeration.
- `theta`: Minimum density a seed must meet to be included in the output.
- `k`: Scaling factor passed to `norm` when computing density.

**Return value**
- A list of `SeedMetrics`, where each entry provides the seed's nodes, triangle count, and density score.

## SeedMetrics structure
Each seed that passes the density filter is represented by a `SeedMetrics` struct with:
- `nodes`: `std::set<int>` containing the nodes in the seed.
- `triangle_count`: Number of triangles in the seed's induced subgraph.
- `density`: Density computed via `norm(triangle_count, seed_size, k)`.
