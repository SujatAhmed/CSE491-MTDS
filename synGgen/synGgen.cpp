#include <algorithm>
#include <cmath> // for exp()
#include <ctime>
#include <fstream>
#include <iostream>
#include <numeric>
#include <random>
#include <set>
#include <unordered_set>
#include <utility>
#include <vector>
#include "../include/norm.h"
#include <bits/stdc++.h>

using namespace std;

void printTriangleDenseSubgraphs(const vector<vector<int>> &subgraphs, float norm_k);
void printTriangleDenseSubgraphs(const vector<vector<int>> &subgraphs,
                                 const vector<vector<int>> &adj, float norm_k);

// Global random number generator
random_device rd;
mt19937 gen(rd());

// Generate a subgraph of given size
vector<int> randSubSetGen(const vector<int> &nodeset, int num_nodes) {
  if (nodeset.empty() || num_nodes <= 0) {
    return {}; // Return empty if nodeset is empty or num_nodes is non-positive
  }

  vector<int> nodeSubset = nodeset;                   // Create a mutable copy
  shuffle(nodeSubset.begin(), nodeSubset.end(), gen); // Using custom generator

  if (num_nodes >= nodeSubset.size()) {
    return nodeSubset; // Return all nodes shuffled
  } else {
    nodeSubset.resize(num_nodes); // Resize to the desired number of elements
    return nodeSubset;            // Return the resized subset
  }
}

// Form a clique within a subgraph
void makeClique(vector<vector<int>> &adj, const vector<int> &subset_nodes) {
  for (int i = 0; i < subset_nodes.size(); ++i) {
    for (int j = i + 1; j < subset_nodes.size(); ++j) {
      int u = subset_nodes[i];
      int v = subset_nodes[j];
      adj[u][v] = 1;
      adj[v][u] = 1;
    }
  }
}

float triangleDensity(const vector<vector<int>> &adj,
                      const vector<int> &subset, float norm_k) {
  int num_triangles = 0;
  int k = subset.size(); // Number of nodes in the subgraph

  // If there are fewer than 3 nodes, no triangles are possible.
  if (k < 3) {
    return 0.0f;
  }

  // Iterate through all unique combinations of three nodes (u, v, w) from the
  // subset
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
  // cout << "\nTriNum: " << num_triangles << " nodes: " << k;
  return norm(num_triangles, k, norm_k);
}

// Remove edges from clique to get to desired threshold
void removeEdgesToMatchTriangleDensity(vector<vector<int>> &adj,
                                       const vector<int> &subset,
                                       float tridense, float norm_k) {
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

  for (const auto &edge : edges) {
    int u = edge.first;
    int v = edge.second;

    // Temporarily remove the edge
    adj[u][v] = 0;
    adj[v][u] = 0;

    float current_density = triangleDensity(adj, subset, norm_k);
    // cout << "\nCD: " << current_density << " td: " << tridense << "\n\n";
    if (current_density < tridense) {
      // Restore the edge if triangle density drops too low
      adj[u][v] = 1;
      adj[v][u] = 1;
    }
  }

  // Final density print
  float final_density = triangleDensity(adj, subset, norm_k);
}

// Connect the nodes to form the sparse graph
void connectNodes(vector<vector<int>> &adj,
                  const vector<vector<int>> &subgraphs,
                  const vector<int> &unused_nodes, double prob_between,
                  double prob_external, double prob_amongNonSub,
                  int max_edges_between_subgraphs, int n,
                  const vector<int> &node_to_subgraph) {

  uniform_real_distribution<> dis(0, 1);

  // Connect subgraphs with a limit on edges to prevent merging
  for (size_t i = 0; i < subgraphs.size(); ++i) {
    for (size_t j = i + 1; j < subgraphs.size(); ++j) {
      uniform_int_distribution<> cap_dist(1, max_edges_between_subgraphs);
      int edges_cap = cap_dist(gen);
      int edges_added = 0;

      for (int u : subgraphs[i]) {
        for (int v : subgraphs[j]) {
          if (edges_added >= edges_cap)
            break;

          bool same_subgraph = false;
          for (int s1 : node_to_subgraph[u])
              for (int s2 : node_to_subgraph[v])
                  if (s1 == s2) same_subgraph = true;

          if (same_subgraph) continue;

          if (dis(gen) < prob_between) {
            adj[u][v] = adj[v][u] = 1;
            edges_added++;
          }
        }
        if (edges_added >= edges_cap)
          break;
      }
    }
  }

  // Connect subgraphs to unused nodes with given probability
  for (const auto &sg : subgraphs) {
    for (int u : sg) {
      for (int v : unused_nodes) {
        bool same_subgraph = false;
        for (int s1 : node_to_subgraph[u])
            for (int s2 : node_to_subgraph[v])
                if (s1 == s2) same_subgraph = true;

        if (same_subgraph) continue;

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
pair<vector<vector<int>>, vector<int>> 
generateSyntheticGraph(int n, int t, double th, double prob_between,
                       double prob_external, double prob_amongNonSub,
                       int max_edges_between_subgraphs,
                       vector<vector<int>> &triangle_subgraphs, float norm_k) {
  vector<int> full_node_set(n);
  iota(full_node_set.begin(), full_node_set.end(), 0);

  vector<int> available_nodes = full_node_set;
  vector<vector<int>> adj(n, vector<int>(n, 0));
  vector<vector<int>> node_to_subgraph(n); // store all subgraph IDs for overlapping

  for (int i = 1; i <= t; ++i) {
    int s = (n / t) - 1;

    uniform_int_distribution<> sizeDist(3, s);  // subgraph size
    uniform_real_distribution<> triDist(th, 0.8);

    int subgraph_size = sizeDist(gen);
    float tridense = triDist(gen);

    vector<int> subset = randSubSetGen(available_nodes, subgraph_size);
    makeClique(adj, subset);
    float current_density = triangleDensity(adj, subset, norm_k);

    removeEdgesToMatchTriangleDensity(adj, subset, tridense, norm_k);

  for (int node : subset) {
      // do NOT remove from available_nodes → allow overlapping
      node_to_subgraph[node].push_back(i); // store this subgraph ID
  }

    triangle_subgraphs.push_back(subset);
  }
  printTriangleDenseSubgraphs(triangle_subgraphs, adj, norm_k);

  connectNodes(adj, triangle_subgraphs, available_nodes, prob_between,
               prob_external, prob_amongNonSub, max_edges_between_subgraphs, n,
               node_to_subgraph);

  return {adj, node_to_subgraph};
}

// Encode an edge u<v as a single number
inline long long encodeEdge(int u, int v) {
    if (u > v) swap(u, v);
    return ((long long)u << 32) | v;
}

// Count common neighbors of u and v in sorted adjacency lists
int countCommonNeighbors(const vector<int> &a, const vector<int> &b) {
    int i = 0, j = 0, cnt = 0;
    while (i < a.size() && j < b.size()) {
        if (a[i] == b[j]) { cnt++; i++; j++; }
        else if (a[i] < b[j]) i++;
        else j++;
    }
    return cnt;
}

// Returns vector of connected subgraphs (each as a set of nodes) of the k-truss
vector<unordered_set<int>> kTruss(vector<vector<int>> adj, int k) {
    int n = adj.size();

    // Sort & dedup adjacency lists
    for (int i = 0; i < n; i++) {
        sort(adj[i].begin(), adj[i].end());
        adj[i].erase(unique(adj[i].begin(), adj[i].end()), adj[i].end());
    }

    // Map edges to indices
    vector<pair<int,int>> edges;
    unordered_map<long long,int> edgeId;
    int id = 0;
    for (int u = 0; u < n; u++) {
        for (int v : adj[u]) {
            if (u < v) {
                long long key = encodeEdge(u,v);
                if (!edgeId.count(key)) {
                    edgeId[key] = id++;
                    edges.push_back({u,v});
                }
            }
        }
    }
    int m = edges.size();

    // Compute initial support (#triangles per edge)
    vector<int> support(m,0);
    vector<bool> removed(m,false);
    for (int e = 0; e < m; e++) {
        int u = edges[e].first, v = edges[e].second;
        support[e] = countCommonNeighbors(adj[u], adj[v]);
    }

    // Copy adjacency for peeling
    vector<vector<int>> adj_copy = adj;
    queue<int> q;

    // Initialize queue with edges having support < k-2
    for (int e = 0; e < m; e++)
        if (support[e] < k-2) q.push(e);

    // Peeling process
    while (!q.empty()) {
        int e = q.front(); q.pop();
        if (removed[e]) continue;
        removed[e] = true;

        int u = edges[e].first, v = edges[e].second;
        // Remove edge from adj_copy
        adj_copy[u].erase(lower_bound(adj_copy[u].begin(), adj_copy[u].end(), v));
        adj_copy[v].erase(lower_bound(adj_copy[v].begin(), adj_copy[v].end(), u));

        // Update support of edges forming triangles with (u,v)
        vector<int> common;
        int i=0, j=0;
        while (i<adj_copy[u].size() && j<adj_copy[v].size()) {
            if (adj_copy[u][i]==adj_copy[v][j]) { common.push_back(adj_copy[u][i]); i++; j++; }
            else if (adj_copy[u][i]<adj_copy[v][j]) i++;
            else j++;
        }

        for (int x : common) {
            long long k1 = encodeEdge(u,x), k2 = encodeEdge(v,x);
            if (edgeId.count(k1)) { int e1 = edgeId[k1]; if (!removed[e1]) { support[e1]--; if (support[e1]<k-2) q.push(e1); } }
            if (edgeId.count(k2)) { int e2 = edgeId[k2]; if (!removed[e2]) { support[e2]--; if (support[e2]<k-2) q.push(e2); } }
        }
    }

    // Build adjacency for remaining edges
    unordered_map<int, vector<int>> sub_adj;
    for (int e = 0; e < m; e++) {
        if (!removed[e]) {
            int u = edges[e].first, v = edges[e].second;
            sub_adj[u].push_back(v);
            sub_adj[v].push_back(u);
        }
    }

    // Extract connected components
    vector<unordered_set<int>> components;
    unordered_set<int> visited;
    for (auto &[u,_] : sub_adj) {
        if (visited.count(u)) continue;
        unordered_set<int> comp;
        queue<int> q2; q2.push(u);
        visited.insert(u);
        while (!q2.empty()) {
            int cur = q2.front(); q2.pop();
            comp.insert(cur);
            for (int v : sub_adj[cur]) {
                if (!visited.count(v)) { visited.insert(v); q2.push(v); }
            }
        }
        components.push_back(comp);
    }

    return components;
}


void printTriangleDenseSubgraphs(const vector<vector<int>> &subgraphs,
                                 const vector<vector<int>> &adj, float norm_k) {
  // cout << "\n--- Generated Triangle-Dense Subgraphs and Densities ---" << endl;
  if (subgraphs.empty()) {
    cout << "No triangle-dense subgraphs were generated." << endl;
    return;
  }

  for (size_t i = 0; i < subgraphs.size(); ++i) {
    const vector<int> &current_subset =
        subgraphs[i]; // Get the current subgraph (subset of nodes)
    float density = triangleDensity(
        adj, current_subset, norm_k); // Calculate density for this subgraph

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


int main(int argc, char *argv[]) {

  if (argc < 9) {
    cerr << "Usage: " << argv[0]
         << " <output_filename.txt> <ground_truth_filename.labels> "
            "<seed_filename.txt> <n> <t> <th> <norm_k> <k-Truss>"
         << endl;
    return 1;
  }
  string filename = argv[1];
  string label_filename = argv[2];
  string seed_filename = argv[3];

  int n = stoi(argv[4]);       // total nodes
  int t = stoi(argv[5]);       // triangle-rich subgraphs
  double th = stod(argv[6]);   // density threshold
  float norm_k = stod(argv[7]); // k constant for normalization
  int k_truss = stod(argv[8]);  // k-truss

  string filePath = filename;
  string labelPath = label_filename;
  string seedPath = seed_filename;

  double prob_between      = (1.0 - th) / (n / double(t));
  double prob_external     = (1.0 - th) * 0.05;
  double prob_amongNonSub  = 0.02;
  int max_edges_between_subgraphs = max(1, int((1.0 - th) * 5));

  vector<vector<int>> triangle_subgraph;

  auto [graph, labels] = generateSyntheticGraph(
      n, t, th, prob_between, prob_external, prob_amongNonSub,
      max_edges_between_subgraphs, triangle_subgraph, norm_k);

  ofstream outfile(filePath);
  for (int i = 0; i < n; ++i) {
    for (int j = i + 1; j < n; ++j) {
      if (graph[i][j]) {
        outfile << i << " " << j << "\n";
      }
    }
  }
  outfile.close();

  ofstream label_file(labelPath);
  for (int i = 0; i < n; ++i) {
      bool has_edge = false;
      for (int j = 0; j < graph[i].size(); ++j) {
          if (graph[i][j] == 1) { has_edge = true; break; }
      }
      if (!has_edge) continue; // skip isolated nodes

      label_file << i << " ";
      if (node_to_subgraph[i].empty())
          label_file << -1 << "\n"; // no subgraph
      else {
          for (size_t j = 0; j < node_to_subgraph[i].size(); ++j) {
              if (j > 0) label_file << " ";
              label_file << node_to_subgraph[i][j];
          }
          label_file << "\n";
      }
  }
  label_file.close();

    vector<vector<int>> adj_list(n);
    for (int i = 0; i < n; ++i)
    for (int j = 0; j < n; ++j)
      if (graph[i][j])
        adj_list[i].push_back(j);
    auto subgraphs = kTruss(adj_list, k_truss);

  ofstream seed_file(seedPath);
  for (const auto &subgraph : subgraphs) {  
      bool first = true;
      for (int node : subgraph) {
          if (!first) seed_file << " ";
          seed_file << node;
          first = false;
      }
      seed_file << "\n";
  }
  seed_file.close();
  
  return 0;
}
