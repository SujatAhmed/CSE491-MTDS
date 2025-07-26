#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <fstream>
#include <random>
#include <ctime>
#include <algorithm>
#include <numeric>
#include <utility>

using namespace std;

void printTriangleDenseSubgraphs(const vector<vector<int>>& subgraphs);
void printTriangleDenseSubgraphs(const vector<vector<int>>& subgraphs, const vector<vector<int>>& adj);

// Global random number generator
random_device rd;
mt19937 gen(rd());

// Generate a subgraph of given size
vector<int> randSubSetGen(const vector<int>& nodeset, int num_nodes) {
    if (nodeset.empty() || num_nodes <= 0) {
        return {}; // Return empty if nodeset is empty or num_nodes is non-positive
    }

    vector<int> nodeSubset = nodeset; // Create a mutable copy
    shuffle(nodeSubset.begin(), nodeSubset.end(), gen); // Using custom generator

    if (num_nodes >= nodeSubset.size()) {
        return nodeSubset; // Return all nodes shuffled
    } else {
        nodeSubset.resize(num_nodes); // Resize to the desired number of elements
        return nodeSubset;            // Return the resized subset
    }
}


// Form a clique within a subgraph
void makeClique(vector<vector<int>>& adj, const vector<int>& subset_nodes) {
    for (int i = 0; i < subset_nodes.size(); ++i) {
        for (int j = i + 1; j < subset_nodes.size(); ++j) {
            int u = subset_nodes[i];
            int v = subset_nodes[j];
            adj[u][v] = 1;
            adj[v][u] = 1;
        }
    }
}

float triangleDensity(const vector<vector<int>>& adj, const vector<int>& subset) {
    int num_triangles = 0;
    int k = subset.size(); // Number of nodes in the subgraph

    // If there are fewer than 3 nodes, no triangles are possible.
    if (k < 3) {
        return 0.0f;
    }

    // Iterate through all unique combinations of three nodes (u, v, w) from the subset
    for (int i = 0; i < k; ++i) {
        for (int j = i + 1; j < k; ++j) {
            for (int l = j + 1; l < k; ++l) {
                int u = subset[i];
                int v = subset[j];
                int w = subset[l];

                // Check if edges (u,v), (v,w), and (w,u) exist
                // The adj matrix stores 1 for an edge, 0 otherwise.
                if (adj[u][v] == 1 && adj[v][w] == 1 && adj[w][u] == 1) {
                    num_triangles++;
                }
            }
        }
    }

    // Calculate the number of possible triangles (k choose 3)
    // Formula: k * (k - 1) * (k - 2) / 6
    // Use long long for intermediate calculation to prevent overflow before division
    int possible_triangles = (int)k * (k - 1) * (k - 2) / 6;

    if (possible_triangles == 0) {
        return 0.0f; // Avoid division by zero if k < 3 (already handled, but good for safety)
    }

    cout << "\n\n Tri: " << num_triangles << "\n pt: " << possible_triangles << "\n\n";

    return static_cast<float>(num_triangles) / possible_triangles;
}



// Remove edges from clique to get to desired threshold
void removeEdgesToMatchTriangleDensity(vector<vector<int>>& adj, const vector<int>& subset, float tridense) {
    vector<pair<int, int>> edges;

    // Collect all edges within the subset
    for (int i = 0; i < subset.size(); ++i) {
        for (int j = i + 1; j < subset.size(); ++j) {
            int u = subset[i];
            int v = subset[j];
            if (adj[u][v] == 1) {
                edges.emplace_back(u, v);
            }
        }
    }

    // Shuffle edges to remove randomly
    shuffle(edges.begin(), edges.end(), gen);

    for (const auto& edge : edges) {
        int u = edge.first;
        int v = edge.second;

        // Temporarily remove the edge
        adj[u][v] = 0;
        adj[v][u] = 0;

        float current_density = triangleDensity(adj, subset);
        cout << "\nCD: " << current_density << " td: " << tridense << "\n\n";
        if (current_density < tridense) {
            // Restore the edge if triangle density drops too low
            adj[u][v] = 1;
            adj[v][u] = 1;
        }
    }

    // Final density print
    float final_density = triangleDensity(adj, subset);
}


// Connect the nodes to form the sparse graph
void connectNodes(vector<vector<int>>& adj,
                  const vector<vector<int>>& subgraphs,
                  const vector<int>& unused_nodes,
                  double prob_between,
                  double prob_external,
                  double prob_amongNonSub,
                  int max_edges_between_subgraphs,
                  int n,
                  const vector<int>& node_to_subgraph
                ) {

    uniform_real_distribution<> dis(0, 1);

    // Connect subgraphs with a limit on edges to prevent merging
    for (size_t i = 0; i < subgraphs.size(); ++i) {
        for (size_t j = i + 1; j < subgraphs.size(); ++j) {
            uniform_int_distribution<> cap_dist(1, max_edges_between_subgraphs);
            int edges_cap = cap_dist(gen);
            int edges_added = 0;

            for (int u : subgraphs[i]) {
                for (int v : subgraphs[j]) {
                    if (edges_added >= edges_cap) break;

                    // Skip if u and v belong to the same subgraph (should never happen here but just in case)
                    if (node_to_subgraph[u] == node_to_subgraph[v] && node_to_subgraph[u] != -1) continue;

                    if (dis(gen) < prob_between) {
                        adj[u][v] = adj[v][u] = 1;
                        edges_added++;
                    }
                }
                if (edges_added >= edges_cap) break;
            }
        }
    }

    // Connect subgraphs to unused nodes with given probability
    for (const auto& sg : subgraphs) {
        for (int u : sg) {
            for (int v : unused_nodes) {
                // Skip if both in same subgraph (should not happen here)
                if (node_to_subgraph[u] == node_to_subgraph[v] && node_to_subgraph[u] != -1) continue;

                if (dis(gen) < prob_external) {
                    adj[u][v] = adj[v][u] = 1;
                }
            }
        }
    }

    // Connect unused nodes among themselves with given probability
    for (size_t i = 0; i < unused_nodes.size(); ++i) {
        for (size_t j = i + 1; j < unused_nodes.size(); ++j) {
            if (dis(gen) < prob_amongNonSub) {
                int u = unused_nodes[i];
                int v = unused_nodes[j];
                adj[u][v] = adj[v][u] = 1;
            }
        }
    }
}

// Main generator
vector<vector<int>> generateSyntheticGraph(int n, int t, double th, double prob_between, double prob_external, double prob_amongNonSub, int max_edges_between_subgraphs) {
    vector<int> full_node_set(n);
    iota(full_node_set.begin(), full_node_set.end(), 0);

    vector<int> available_nodes = full_node_set;
    vector<vector<int>> adj(n, vector<int>(n, 0));
    vector<vector<int>> triangle_subgraphs;
    vector<int> node_to_subgraph(n, -1);  // -1 = not assigned

    for (int i = 1; i <= t; ++i) {
        int s = (n / t) - 1;
        int subgraph_size = (rand() % (s - 3 + 1)) + 3;
        cout << "i:" << i << "\n";

        vector<int> subset = randSubSetGen(available_nodes, subgraph_size);
        makeClique(adj, subset);
        float current_density = triangleDensity(adj, subset);
        cout << "\n\nafter forming cl: " << current_density << "\n\n";

        double tridense = th + ((double)rand() / RAND_MAX) * (0.8 - th);

        removeEdgesToMatchTriangleDensity(adj, subset, tridense);

        for (int node : subset) {
            available_nodes.erase(remove(available_nodes.begin(), available_nodes.end(), node), available_nodes.end());
            node_to_subgraph[node] = i;  // Mark the TDS index
        }

        triangle_subgraphs.push_back(subset);
    }
    printTriangleDenseSubgraphs(triangle_subgraphs, adj);

    connectNodes(adj, triangle_subgraphs, available_nodes, prob_between, prob_external, prob_amongNonSub, max_edges_between_subgraphs, n, node_to_subgraph);


    return adj;
}

void printTriangleDenseSubgraphs(const vector<vector<int>>& subgraphs, const vector<vector<int>>& adj) {
    cout << "\n--- Generated Triangle-Dense Subgraphs and Densities ---" << endl;
    if (subgraphs.empty()) {
        cout << "No triangle-dense subgraphs were generated." << endl;
        return;
    }

    for (size_t i = 0; i < subgraphs.size(); ++i) {
        const vector<int>& current_subset = subgraphs[i]; // Get the current subgraph (subset of nodes)
        float density = triangleDensity(adj, current_subset); // Calculate density for this subgraph

        cout << "Subgraph " << i + 1 << " (Nodes): [";
        for (size_t j = 0; j < current_subset.size(); ++j) {
            cout << current_subset[j];
            if (j < current_subset.size() - 1) {
                cout << ", ";
            }
        }
        cout << "] - Density: " << density << endl; // Print the density
    }
    cout << "----------------------------------------------------" << endl;
}

int main() {
    int n = 25;       // total nodes
    int t = 2;         // triangle-rich subgraphs
    double th = 0.45;   // density threshold
    double prob_between = 0.01;
    double prob_external = 0.05;
    double prob_amongNonSub = 0.05;
    int max_edges_between_subgraphs = 2;

    string filename;
    cout << "Enter output file name (with .edges extension): ";
    cin >> filename;

    vector<vector<int>> graph = generateSyntheticGraph(n, t, th, prob_between, prob_external, prob_amongNonSub, max_edges_between_subgraphs);

    ofstream outfile(filename);
    for (int i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (graph[i][j]) {
                outfile << i << " " << j << "\n";
            }
        }
    }
    outfile.close();

    cout << "Graph saved to " << filename << endl;

    return 0;
} 