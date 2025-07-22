#include <iostream>
#include <vector>
#include <set>
#include <unordered_set>
#include <fstream>
#include <random>
#include <ctime>
#include <algorithm>

using namespace std;

using Edge = pair<int, int>;

void add_triangle_dense_subgraph(int start_node, int size, double threshold, set<Edge> &edges) {
    int max_triangles = size * (size - 1) * (size - 2) / 6;
    int target_triangles = static_cast<int>(threshold * size);

    int triangle_count = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(start_node, start_node + size - 1);

    while (triangle_count < target_triangles) {
        int a = dis(gen), b = dis(gen), c = dis(gen);
        if (a == b || b == c || a == c) continue;
        vector<int> tri = {a, b, c};
        sort(tri.begin(), tri.end());

        Edge e1 = {tri[0], tri[1]};
        Edge e2 = {tri[1], tri[2]};
        Edge e3 = {tri[0], tri[2]};

        if (edges.count(e1) && edges.count(e2) && edges.count(e3)) continue;

        edges.insert(e1);
        edges.insert(e2);
        edges.insert(e3);
        triangle_count++;
    }
}

void generate_sparse_base_graph(int total_nodes, int dense_nodes_start,
                                double edge_prob, set<Edge> &edges) {
    unordered_set<int> connected;
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);

    for (int i = 0; i < total_nodes; ++i) {
        for (int j = i + 1; j < total_nodes; ++j) {
            if (i >= dense_nodes_start || j >= dense_nodes_start) {
                if (dis(gen) < edge_prob) {
                    edges.insert({i, j});
                    connected.insert(i);
                    connected.insert(j);
                }
            }
        }
    }

    int required_connected = total_nodes * 0.7;
    vector<int> disconnected;
    for (int i = 0; i < total_nodes; ++i) {
        if (connected.count(i) == 0) disconnected.push_back(i);
    }

    shuffle(disconnected.begin(), disconnected.end(), gen);
    int idx = 0;
    while ((int)connected.size() < required_connected && idx < disconnected.size()) {
        int u = disconnected[idx++];
        int v = gen() % total_nodes;
        while (v == u || edges.count({min(u, v), max(u, v)})) {
            v = gen() % total_nodes;
        }
        edges.insert({min(u, v), max(u, v)});
        connected.insert(u);
        connected.insert(v);
    }
}

int main() {
    srand(time(nullptr));

    int total_nodes;
    string input;
    cout << "Enter number of nodes (or type 'random'): ";
    cin >> input;

    if (input == "random") {
        total_nodes = rand() % 9901 + 100; // Between 100 and 10000
        cout << "Randomly selected number of nodes: " << total_nodes << endl;
    } else {
        total_nodes = stoi(input);
    }

    int num_dense_subgraphs;
    double threshold;
    cout << "Enter number of triangle-dense subgraphs to implant: ";
    cin >> num_dense_subgraphs;
    cout << "Enter triangle density threshold: ";
    cin >> threshold;

    string output_filename;
    cout << "Enter output filename (with .edges extension): ";
    cin >> output_filename;

    set<Edge> edges;
    int nodes_per_subgraph = total_nodes / (num_dense_subgraphs + 1);
    int next_dense_start = 0;

    for (int i = 0; i < num_dense_subgraphs; ++i) {
        add_triangle_dense_subgraph(next_dense_start, nodes_per_subgraph, threshold, edges);
        next_dense_start += nodes_per_subgraph;
    }

    // Fill rest of graph with sparse base
    double edge_prob;
    cout << "Enter the Graph Density: \n" << "Cheat Sheet: \n< 0.001	Ultra Sparse \n0.001–0.01	Very Sparse\n0.01–0.05	Sparse\n0.05–0.1	Semi-Sparse\n0.1–0.3	Moderate Density\n0.3–0.6	Dense\n> 0.6	Very Dense\n= 1.0	Complete Graph\n";
    cin >> edge_prob; 
    generate_sparse_base_graph(total_nodes, next_dense_start, edge_prob, edges);

    // Save to user-named file
    ofstream outfile(output_filename);
    if (!outfile.is_open()) {
        cerr << "❌ Failed to open output file: " << output_filename << endl;
        return 1;
    }

    for (const auto &e : edges) {
        outfile << e.first << " " << e.second << "\n";
    }
    outfile.close();

    cout << "✅ Graph saved to: " << output_filename << " with " << edges.size() << " edges.\n";
    return 0;
}
