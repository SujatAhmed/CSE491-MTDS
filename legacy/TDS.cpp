// #include "TriAl.cpp"
// #include <iostream>
// #include <fstream>
// #include <unordered_set>
// #include <unordered_map>
// #include <vector>
// #include <set>
// #include <algorithm>
// #include <sstream>
//
// using namespace std;
//
// using NodeSet = unordered_set<int>;
//
// double triangleDensity(const unordered_map<int, unordered_set<int>>& adj, const NodeSet& nodes) {
//     int s = nodes.size();
//     if (s < 3) return 0.0;
//
//     // Reconstruct edge list from the induced subgraph
//     vector<pair<int, int>> edgeList;
//     for (int u : nodes) {
//         for (int v : adj.at(u)) {
//             if (nodes.count(v) && u < v) {
//                 edgeList.emplace_back(u, v);
//             }
//         }
//     }
//
//     int max_node = *max_element(nodes.begin(), nodes.end());
//     int tri = countTriangles(max_node + 1, edgeList);
//     double maxTri = (s * (s - 1) * (s - 2)) / 6.0;
//
//     return tri / maxTri;
// }
//
// NodeSet triangleDenseSubgraph(const unordered_map<int, unordered_set<int>>& adj,
//                                const NodeSet& seedSet, double theta) {
//     NodeSet Vt = seedSet, Vtp1 = seedSet;
//     int t = 0;
//
//     do {
//         Vt = Vtp1;
//
//         // --- Add step ---
//         for (int u : Vt) {
//             for (int v : adj.at(u)) {
//                 if (Vt.count(v)) continue;
//
//                 NodeSet Vprime = Vt;
//                 Vprime.insert(v);
//
//                 double f_new = triangleDensity(adj, Vprime);
//                 if (f_new >= theta && f_new > triangleDensity(adj, Vtp1)) {
//                     Vtp1 = Vprime;
//                 }
//             }
//         }
//
//         // --- Remove step ---
//         for (int u : Vtp1) {
//             NodeSet Vprime = Vtp1;
//             Vprime.erase(u);
//
//             double f_new = triangleDensity(adj, Vprime);
//             if (f_new >= theta && f_new > triangleDensity(adj, Vtp1)) {
//                 Vtp1 = Vprime;
//             }
//         }
//
//         t++;
//     } while (Vt != Vtp1);
//
//     return Vtp1;
// }
//
// unordered_map<int, unordered_set<int>> readEdgeFile(const string& filename) {
//     ifstream file(filename);
//     unordered_map<int, unordered_set<int>> adj;
//     string line;
//
//     while (getline(file, line)) {
//         if (line.empty() || line[0] == '#') continue;
//         int u, v;
//         stringstream ss(line);
//         ss >> u >> v;
//         if (u == v) continue;
//         adj[u].insert(v);
//         adj[v].insert(u);
//     }
//     return adj;
// }
//
// int main(int argc, char* argv[]) {
//     if (argc < 3) {
//         cerr << "Usage: ./td_dense <graph.edge> <theta> [seed nodes...]\n";
//         return 1;
//     }
//
//     string filename = argv[1];
//     double theta = stod(argv[2]);
//
//     NodeSet seedSet;
//     for (int i = 3; i < argc; ++i) {
//         seedSet.insert(stoi(argv[i]));
//     }
//
//     auto adj = readEdgeFile(filename);
//     NodeSet result = triangleDenseSubgraph(adj, seedSet, theta);
//
//     cout << "Final triangle-dense subgraph (|V| = " << result.size() << "):\n";
//     for (int v : result) {
//         cout << v << " ";
//     }
//     cout << endl;
//
//     return 0;
// }
