#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <cmath>
#include <algorithm>
#include <numeric>

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
    while(head < queue.size()){
        unsigned u = queue[head++];
        if(current_adj.count(u)){
            for(unsigned v : current_adj.at(u)){
                if(visited.find(v) == visited.end()){
                    visited.insert(v);
                    component_nodes.insert(v);
                    queue.push_back(v);
                }
            }
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <edgelist.txt> <theta>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    double theta = std::stod(argv[2]);

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    unsigned u, v;
    unsigned max_node = 0;
    std::unordered_map<unsigned, std::unordered_set<unsigned>> original_adj;

    // Step 1: Read graph and build adjacency list using unordered_set for faster lookups
    while (file >> u >> v) {
        original_adj[u].insert(v);
        original_adj[v].insert(u);
        max_node = std::max({max_node, u, v});
    }
    file.close();

    std::cout << "Initial number of nodes: " << max_node + 1 << std::endl;
    std::cout << "Reading graph completed." << std::endl;

    // Step 2 & 3: Find triangles and remove nodes not involved in any triangle
    std::unordered_map<unsigned, std::vector<unsigned>> filtered_adj;
    std::unordered_map<unsigned, unsigned> subgraph_triangle_counts;

    for (const auto& pair_u : original_adj) {
        unsigned node_u = pair_u.first;
        if (pair_u.second.size() < 2) continue; // Skip nodes with degree < 2
        
        for (unsigned node_v : pair_u.second) {
            if (node_u > node_v) continue;

            for (unsigned node_w : original_adj[node_v]) {
                if (node_w > node_u && original_adj.at(node_u).count(node_w)) {
                    // Triangle (u, v, w) found
                    filtered_adj[node_u].push_back(node_v);
                    filtered_adj[node_v].push_back(node_u);
                    filtered_adj[node_u].push_back(node_w);
                    filtered_adj[node_w].push_back(node_u);
                    filtered_adj[node_v].push_back(node_w);
                    filtered_adj[node_w].push_back(node_v);
                }
            }
        }
    }

    // Now, find connected components in the filtered graph
    std::vector<std::unordered_set<unsigned>> subgraphs;
    std::unordered_set<unsigned> visited_nodes;
    
    std::cout << "Finding subgraphs..." << std::endl;

    for (const auto& pair : filtered_adj) {
        unsigned node = pair.first;
        if(visited_nodes.find(node) == visited_nodes.end()){
            std::unordered_set<unsigned> component;
            find_subgraphs(node, filtered_adj, visited_nodes, component);
            subgraphs.push_back(component);
        }
    }
    
    // Clean up filtered_adj to remove duplicate edges and sort for density calculation
    for(auto& pair : filtered_adj) {
        std::sort(pair.second.begin(), pair.second.end());
        pair.second.erase(std::unique(pair.second.begin(), pair.second.end()), pair.second.end());
    }

    // Step 4 & 5: Calculate density and filter
    std::cout << "\nFound " << subgraphs.size() << " subgraphs." << std::endl;
    std::cout << "Filtering subgraphs with density >= " << theta << std::endl;
    
    std::ofstream edges_out("subgraphs.edges");
    std::ofstream adj_out("subgraphs.adj.txt");
    int subgraph_id = 0;

    for (const auto& subgraph_nodes : subgraphs) {
        if (subgraph_nodes.size() < 3) continue;

        unsigned long long subgraph_triangles = 0;
        
        // Count triangles in the subgraph
        for(unsigned node_u : subgraph_nodes) {
            for(unsigned node_v : filtered_adj[node_u]) {
                if (node_u < node_v && subgraph_nodes.count(node_v)) {
                    // Find common neighbors
                    for(unsigned node_w : filtered_adj[node_v]) {
                        if (node_v < node_w && subgraph_nodes.count(node_w)) {
                            if (std::find(filtered_adj[node_u].begin(), filtered_adj[node_u].end(), node_w) != filtered_adj[node_u].end()) {
                                subgraph_triangles++;
                            }
                        }
                    }
                }
            }
        }
        subgraph_triangles /= 3;

        // Triangle density calculation
        double density = 0.0;
        if (subgraph_nodes.size() > 0) {
            double value = static_cast<double>(subgraph_triangles) / subgraph_nodes.size();
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

            // Export subgraph details
            adj_out << "Subgraph " << subgraph_id << ": Nodes(" << subgraph_nodes.size() << ")\n";
            for(unsigned node : subgraph_nodes){
                adj_out << node << ": ";
                for(unsigned neighbor : filtered_adj[node]){
                    if (subgraph_nodes.count(neighbor)) {
                        adj_out << neighbor << " ";
                        if (node < neighbor) {
                            edges_out << node << " " << neighbor << "\n";
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