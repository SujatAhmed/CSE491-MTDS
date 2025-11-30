# Main Program Overview and Usage

This guide explains how `main.cpp` orchestrates the graph processing pipeline and how to run the program from the command line.

## What the program does
- **Argument parsing:** The program expects five key-value arguments (`graph`, `seed`, `density`, `temperature`, `alpha`) and terminates early with a usage message if any are missing.
- **Input locations:** It builds file paths using the base directory `/home/sujat/projects/cse491/TestGraphs/Graphs/`, looking for the graph file directly under that directory and seed files under the nested `seeds/` folder.
- **Graph loading:** `generateAdjacencyMap` reads the graph file and produces an adjacency map used throughout the run.
- **Seed ingestion:** `read_seeds_with_density` loads candidate seed sets from the seed file, filtering them by the requested density threshold.
- **Processing loop:** For each seed set, the program prints its node members, triangle count, and density, then refines the seed with `simulated_annealing_v` to search for triangle-dense subgraphs. Each maximal subgraph is collected in memory.

## Required arguments
Pass each argument as `name=value`:

- `graph` – Graph filename relative to `/home/sujat/projects/cse491/TestGraphs/Graphs/` (e.g., `graph1.txt`).
- `seed` – Seed filename relative to `/home/sujat/projects/cse491/TestGraphs/Graphs/seeds/`.
- `density` – Floating-point density threshold for seed filtering.
- `temperature` – Initial temperature (integer) for simulated annealing.
- `alpha` – Cooling rate (float) for simulated annealing.

If any argument is missing, the program prints a usage hint and exits.

## Example usage
From the repository root, run:

```bash
./main graph=graph1.txt seed=seed1.txt density=0.5 temperature=100 alpha=0.95
```

The program echoes the parsed arguments with colored labels, loads the graph and seeds, then prints each seed’s nodes alongside its triangle count and density before running simulated annealing to expand it.

## Output notes
- File locations are hard-coded; ensure the graph and seed files exist under `/home/sujat/projects/cse491/TestGraphs/Graphs/` and its `seeds/` subdirectory.
- The program currently reports subgraph members and metrics to standard output and stores the maximal subgraphs internally; adapt `main.cpp` if you need to persist results elsewhere.
