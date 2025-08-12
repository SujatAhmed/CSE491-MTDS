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
#include <cmath> // for exp()

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

//Sigmoid Function
float sigmoid(float x) {
    return 1.0f / (1.0f + exp(-x));
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

    float rawDensity = static_cast<float>(num_triangles) / k;
    return sigmoid(rawDensity);
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
        if (current_density < tridense) {
            // Restore the edge if triangle density drops too low
            adj[u][v] = 1;
            adj[v][u] = 1;
        }
    }

    // Final density print
    float final_density = triangleDensity(adj, subset);
}

// Function to calculate degrees of all nodes
vector<int> calculateDegrees(int n, const vector<vector<int>>& adj) {
    vector<int> degrees(n, 0);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            if (adj[i][j] == 1) {
                degrees[i]++;
            }
        }
    }
    return degrees;
}

// Main generator
vector<vector<int>> generateSyntheticGraph(int n, int t, double th) {
    vector<int> full_node_set(n);
    iota(full_node_set.begin(), full_node_set.end(), 0);

    vector<int> available_nodes = full_node_set;
    vector<vector<int>> adj(n, vector<int>(n, 0));
    vector<vector<int>> triangle_subgraphs;
    vector<int> node_to_subgraph(n, -1); // -1 = not assigned

    for (int i = 1; i <= t; ++i) {
        int s = (n / t) - 1;
        int subgraph_size = (rand() % (s - 3 + 1)) + 3;

        vector<int> subset = randSubSetGen(available_nodes, subgraph_size);
        makeClique(adj, subset);
        double tridense = th + ((double)rand() / RAND_MAX) * (1 - th);
        removeEdgesToMatchTriangleDensity(adj, subset, tridense);

        for (int node : subset) {
            available_nodes.erase(remove(available_nodes.begin(), available_nodes.end(), node), available_nodes.end());
            node_to_subgraph[node] = i; // Mark the TDS index
        }

        triangle_subgraphs.push_back(subset);
    }
    printTriangleDenseSubgraphs(triangle_subgraphs, adj);

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
    int n = 40;        // total nodes
    int t = 3;         // triangle-rich subgraphs
    double th = 0.5;    // density threshold

    string filename;
    cout << "Enter output file name (with .edges extension): ";
    cin >> filename;

    vector<vector<int>> graph = generateSyntheticGraph(n, t, th);

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