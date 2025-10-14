#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <numeric>

// A simple structure to represent an edge
struct Edge {
    unsigned s;
    unsigned t;
};

// Function to perform a Breadth-First Search (BFS) to find connected components
void find_subgraphs(unsigned start_node,
                    const std::unordered_map<unsigned, std::vector<unsigned>>& current_adj,
                    std::unordered_set<unsigned>& visited,
                    std::unordered_set<unsigned>& component_nodes) {
    std::vector<unsigned> queue;
    queue.push_back(start_node);
    visited.insert(start_node);
    component_nodes.insert(start_node);

    size_t head = 0;
    while (head < queue.size()) {
        unsigned u = queue[head++];
        if (current_adj.count(u)) {
            for (unsigned v : current_adj.at(u)) {
                if (visited.find(v) == visited.end()) {
                    visited.insert(v);
                    component_nodes.insert(v);
                    queue.push_back(v);
                }
            }
        }
    }
}

// Function to check if an edge (u, v) is part of a triangle
bool is_part_of_triangle(unsigned u, unsigned v, const std::unordered_map<unsigned, std::vector<unsigned>>& adj) {
    if (!adj.count(u) || !adj.count(v)) {
        return false;
    }
    const auto& neighbors_u = adj.at(u);
    const auto& neighbors_v = adj.at(v);

    std::unordered_set<unsigned> u_neighbors_set(neighbors_u.begin(), neighbors_u.end());
    for (unsigned w : neighbors_v) {
        if (w != u && u_neighbors_set.count(w)) {
            return true;
        }
    }
    return false;
}

int main(int argc, char* argv[]) {
    // Correct usage message for command-line arguments
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <edgelist.edges> <theta> <dv>" << std::endl;
        return 1;
    }

        std::string outname;
        std::cout << "Enter output file names: ";
        std::cin >> outname;
    std::string filename = argv[1];
    double theta = std::stod(argv[2]);
    unsigned dv = std::stoul(argv[3]);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    unsigned u, v;
    unsigned max_node = 0;
    std::unordered_map<unsigned, std::vector<unsigned>> original_adj;
    std::unordered_map<unsigned, unsigned> node_degrees;
    std::unordered_set<unsigned> all_nodes;

    // Step 1: Read graph and build adjacency list
    std::vector<Edge> original_edges;
    while (file >> u >> v) {
        original_edges.push_back({u, v});
        original_adj[u].push_back(v);
        original_adj[v].push_back(u);
        node_degrees[u]++;
        node_degrees[v]++;
        all_nodes.insert(u);
        all_nodes.insert(v);
        max_node = std::max({max_node, u, v});
    }
    file.close();

    std::cout << "Initial number of nodes: " << all_nodes.size() << std::endl;
    std::cout << "Initial number of edges: " << original_edges.size() << std::endl;

    // Step 2: Filter out nodes based on degree dv and remove isolated nodes.
    std::unordered_map<unsigned, std::vector<unsigned>> degree_filtered_adj;
    std::unordered_set<unsigned> retained_nodes;

    std::cout << "Filtering nodes with degree less than " << dv << "..." << std::endl;

    for (unsigned node : all_nodes) {
        if (node_degrees.count(node) && node_degrees[node] >= dv) {
            retained_nodes.insert(node);
        }
    }

    for (unsigned node : retained_nodes) {
        for (unsigned neighbor : original_adj[node]) {
            if (retained_nodes.count(neighbor)) {
                degree_filtered_adj[node].push_back(neighbor);
            }
        }
    }

    std::cout << "Nodes after degree filtering: " << retained_nodes.size() << std::endl;

    // Step 3: Remove edges that are not part of any triangle
    std::unordered_map<unsigned, std::vector<unsigned>> triangle_filtered_adj;
    std::unordered_set<unsigned> nodes_in_triangle_network;
    unsigned edges_in_triangle_network = 0;

    for (const auto& pair_u : degree_filtered_adj) {
        unsigned node_u = pair_u.first;
        for (unsigned node_v : pair_u.second) {
            if (node_u < node_v) {
                if (is_part_of_triangle(node_u, node_v, degree_filtered_adj)) {
                    triangle_filtered_adj[node_u].push_back(node_v);
                    triangle_filtered_adj[node_v].push_back(node_u);
                    nodes_in_triangle_network.insert(node_u);
                    nodes_in_triangle_network.insert(node_v);
                    edges_in_triangle_network++;
                }
            }
        }
    }

    std::cout << "Nodes in triangle network: " << nodes_in_triangle_network.size() << std::endl;
    std::cout << "Edges in triangle network: " << edges_in_triangle_network << std::endl;

    // Now find connected components (subgraphs) in the final filtered graph
    std::vector<std::unordered_set<unsigned>> subgraphs;
    std::unordered_set<unsigned> visited_nodes;

    for(unsigned node : nodes_in_triangle_network){
        if(visited_nodes.find(node) == visited_nodes.end()){
            std::unordered_set<unsigned> component;
            find_subgraphs(node, triangle_filtered_adj, visited_nodes, component);
            subgraphs.push_back(component);
        }
    }

    // Step 4: Calculate density and filter
    std::cout << "\nFound " << subgraphs.size() << " subgraphs." << std::endl;
    std::cout << "Filtering subgraphs with density >= " << theta << std::endl;

        std::ofstream edges_out(outname + "_clusters.txt");
        std::ofstream adj_out(outname + "_clusters.txt", std::ios::app); // keep both in one file
        std::ofstream idmap_out(outname + "_clusterids.txt");    int subgraph_id = 0;

    for (const auto& subgraph_nodes : subgraphs) {
        if (subgraph_nodes.size() < 3) continue;

        unsigned long long subgraph_triangles = 0;
        unsigned num_subgraph_nodes = subgraph_nodes.size();

        for (unsigned node_u : subgraph_nodes) {
            if (triangle_filtered_adj.count(node_u)) {
                for (unsigned node_v : triangle_filtered_adj.at(node_u)) {
                    if (node_u < node_v && subgraph_nodes.count(node_v)) {
                        for (unsigned node_w : triangle_filtered_adj.at(node_v)) {
                            if (node_v < node_w && subgraph_nodes.count(node_w)) {
                                bool uw_edge_exists = false;
                                for (unsigned neighbor_of_u : triangle_filtered_adj.at(node_u)) {
                                    if (neighbor_of_u == node_w) {
                                        uw_edge_exists = true;
                                        break;
                                    }
                                }
                                if (uw_edge_exists) {
                                    subgraph_triangles++;
                                }
                            }
                        }
                    }
                }
            }
        }
        subgraph_triangles /= 3;

        // Triangle density calculation
        double density = 0.0;
        if (num_subgraph_nodes > 0) {
            double value = static_cast<double>(subgraph_triangles) / num_subgraph_nodes;
            density = 1.0 / (1.0 + exp(-value));
        }

        if (density >= theta) {
            subgraph_id++;
            std::cout << "Subgraph " << subgraph_id << ": [";
            bool first = true;
            for (unsigned node : subgraph_nodes) {
                if (!first) std::cout << ", ";
                std::cout << node;
                first = false;
            }
            std::cout << "] density: " << density << std::endl;

            // Export adjacency list and edges for the dense subgraph
            adj_out << "Subgraph " << subgraph_id << ": Nodes(" << num_subgraph_nodes << ")\n";
            for (unsigned node : subgraph_nodes) {
                adj_out << node << ": ";
                if (triangle_filtered_adj.count(node)) {
                    for (unsigned neighbor : triangle_filtered_adj.at(node)) {
                        if (subgraph_nodes.count(neighbor)) {
                            adj_out << neighbor << " ";
                            if (node < neighbor) {
                                idmap_out << node << " " << subgraph_id << "\n";
                            }
                        }
                    }
                }
                adj_out << "\n";
            }
            adj_out << "\n";
        }
    }

    edges_out.close();
    adj_out.close();

    return 0;
}
