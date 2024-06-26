#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <unordered_set>
#include <iterator>
#include <iomanip>
#include <set>
#include <chrono>
#include <random>

typedef struct {
    std::string name;
    std::string artist;
    double danceability;
    double energy;
    double loudness;
    double valence;
} node;

typedef struct {
    unsigned source;
    unsigned target;
} edge;

typedef struct {
    std::vector<edge> edges; // list of edges
    std::vector<node> nodes; // list of nodes
    std::vector<unsigned> ns; // ns[l]: number of nodes in G_l
    std::vector<unsigned> d; // d[l]: degrees of G_l
    std::vector<unsigned> cd; // cumulative degree: (starts with 0) length=n+1
    std::vector<unsigned> adj; // truncated list of neighbors
    std::vector<unsigned> lab; // lab[i] label of node i
    std::vector<std::vector<unsigned>> sub; // sub[l]: nodes in G_l
    unsigned n() const {
        return nodes.size(); // no. of nodes
    }
    unsigned e() const {
        return edges.size(); // no. of edges
    }
} graph;
void buildGraph(graph &g) {
    node n;
    std::ifstream file("playlist_audio_features.txt");
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        return;
    }
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string name, artist, danceability, energy, loudness, valence;
        if (std::getline(iss, name, ';') &&
            std::getline(iss, artist, ';') &&
            std::getline(iss, danceability, ';') &&
            std::getline(iss, energy, ';') &&
            std::getline(iss, loudness, ';') &&
            std::getline(iss, valence, ';')) {
            try {
                // delete whitespaces
                name = name.substr(name.find_first_not_of(" \t\r\n"), name.find_last_not_of(" \t\r\n") + 1);
                artist = artist.substr(artist.find_first_not_of(" \t\r\n"), artist.find_last_not_of(" \t\r\n") + 1);
                danceability = danceability.substr(danceability.find_first_not_of(" \t\r\n"), danceability.find_last_not_of(" \t\r\n") + 1);
                energy = energy.substr(energy.find_first_not_of(" \t\r\n"), energy.find_last_not_of(" \t\r\n") +1);
                loudness = loudness.substr(loudness.find_first_not_of(" \t\r\n"), loudness.find_last_not_of(" \t\r\n") + 1);
                valence = valence.substr(valence.find_first_not_of(" \t\r\n"), valence.find_last_not_of(" \t\r\n") + 1);

                n.name = name;
                n.artist = artist;
                n.danceability = std::stod(danceability);
                n.energy = std::stod(energy);
                n.loudness = std::stod(loudness);
                n.valence = std::stod(valence);
                g.nodes.push_back(n);
            } catch (const std::invalid_argument &e) {
                std::ofstream file;
                file.open("error-log.txt");
                file << "Invalid argument: " << e.what() << " in line: " << line << std::endl;
            } catch (const std::out_of_range &e) {
                std::ofstream file;
                file.open("error-log.txt");
                file << "Out of range: " << e.what() << " in line: " << line << std::endl;
            }
        }
    }
    file.close();
}

void readEdgelist(graph&g, int mood)
{
    std::string filename = "graph" + std::to_string(mood) + ".txt";
    std::ifstream file(filename);
    edge e;
    if (!file.is_open()) {
        std::cerr << "Unable to open file" << std::endl;
        }
    while(file >> e.source >> e.target)
    {
        g.edges.push_back(e);
    }
}

void makeAdj(graph &g) {
    unsigned md = 0;
    std::vector<unsigned> d0(g.n(), 0);
    for (auto &e : g.edges) {
        d0[e.source]++;
        d0[e.target]++;
    }
    for (auto &x : d0) {
        md = std::max(x, md);
    }
    std::vector<unsigned> bin(md + 1, 0);
    std::vector<unsigned> pos(g.n());
    std::vector<unsigned> vert(g.n());
    for (unsigned i = 0; i < g.n(); i++) {
        bin[d0[i]]++;
    }
    unsigned start = 0;
    for (unsigned i = 0; i <= md; i++) {
        unsigned num = bin[i];
        bin[i] = start;
        start += num;
    }
    for (unsigned i = 0; i < g.n(); i++) {
        pos[i] = bin[d0[i]];
        vert[pos[i]] = i;
        bin[d0[i]]++;
    }
    g.d = vert;
    std::vector<unsigned> cd0(g.n() + 1, 0);
    std::vector<unsigned> adj0(2 * g.e(), 0);
    g.adj.resize(2 * g.e());
    for (unsigned i = 1; i < g.n() + 1; i++) {
        cd0[i] = cd0[i - 1] + d0[i - 1];
        d0[i - 1] = 0;
    }
    for (unsigned i = 0; i < g.e(); i++) {
        adj0[cd0[g.edges[i].source] + d0[g.edges[i].source]++] = g.edges[i].target;
        adj0[cd0[g.edges[i].target] + d0[g.edges[i].target]++] = g.edges[i].source;
    }
    g.adj = adj0;
    g.cd = cd0;
}

std::unordered_set<int> neighbors(const graph &g, unsigned n) {
    std::unordered_set<int> neighbor;
    unsigned start = g.cd[n];
    unsigned end = g.cd[n + 1];
    for (unsigned i = start; i < end; i++) {
        unsigned j = g.adj[i];
        if (j != n) {
            neighbor.insert(j);
        }
    }
    return neighbor;
}

void bronKerboschPivot(const graph& g, 
                       std::unordered_set<int>& R, 
                       std::unordered_set<int>& P, 
                       std::unordered_set<int>& X, 
                       std::vector<std::unordered_set<int>>& cliques) {
    if (P.empty() && X.empty()) {
        cliques.push_back(R);
        return;
    }

    // Choose a pivot vertex from P union X
    int pivot = -1;
    std::unordered_set<int> P_union_X;
    P_union_X.insert(P.begin(), P.end());
    P_union_X.insert(X.begin(), X.end());
    if (!P_union_X.empty()) {
        for(unsigned i = g.n(); i >= 0; i--)
        {
            if(P_union_X.find(g.d[i]) != P_union_X.end())
            {
                pivot = g.d[i];
                break;
            }
        }
    }

    std::unordered_set<int> P_prime;
    for (int v : P) {
        if (neighbors(g, pivot).find(v) == neighbors(g, pivot).end()) {
            P_prime.insert(v);
        }
    }

    for (int v : P_prime) {
        R.insert(v);
        std::unordered_set<int> N_v = neighbors(g, v);
        std::unordered_set<int> P_inter_Nv, X_inter_Nv;

        std::set_intersection(P.begin(), P.end(), N_v.begin(), N_v.end(),
                              std::inserter(P_inter_Nv, P_inter_Nv.begin()));
        std::set_intersection(X.begin(), X.end(), N_v.begin(), N_v.end(),
                              std::inserter(X_inter_Nv, X_inter_Nv.begin()));

        bronKerboschPivot(g, R, P_inter_Nv, X_inter_Nv, cliques);

        R.erase(v);
        P.erase(v);
        X.insert(v);
    }
}

bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

int main() {
    std::cout << "Checking preliminaries..." << std::endl;
    std::string files[5] = {"graph0.txt", "graph1.txt", "graph2.txt", "graph3.txt", "graph4.txt"};
    for(unsigned i = 0; i < 5; i++)
    {
        if(!fileExists(files[i]))
        {
            std::cout << "File: " << files[i] << " does not exist!" << std::endl;
            std::cout << "Attempting to compile file..." << std::endl;
            int compile_status = system("g++ createedgelist.cpp -o createedgelist");
            if(compile_status == 0)
            {
                std::ostringstream command;
                command << "createedgelist " << i;
                int run_status = system(command.str().c_str());
                if(run_status == 0)
                {
                    std::cout << "File: " << files[i] << " created!" << std::endl;
                }
                else{
                    std::cerr << "Error: Failed to execute createedgelist program." << std::endl;
                    return 0;
                }
            }
            else 
            {
                std::cerr << "Error: Compilation of createedgelist.cpp failed." << std::endl;
                return 0;
            }
        }
        if(!fileExists("fusedGraph.txt"))
        {
            std::cout << "File: fusedGraph.txt not found" << std::endl;
            std::cout << "Fusing Graphs....." << std::endl;
            int compile_status = system("g++ graph_fusion.cpp -o graph_fusion");
            if(compile_status == 0)
            {
                int run_status = system("graph_fusion ");
                if(run_status == 0)
                {
                    std::cout << "File: fusedGraph.txt created!" << std::endl;
                }
                else{
                    std::cerr << "Error: Failed to execute graph_fusion program." << std::endl;
                    return 0;
                }
            }
            else 
            {
                std::cerr << "Error: Compilation of graph_fusion.cpp failed." << std::endl;
                return 0;
            }
        }
        
    }
    graph g;
    buildGraph(g);

    // Debug: Print nodes
    // std::cout << "Nodes:" << std::endl;
    // for (const auto &n : g.nodes) {
    //     std::cout << std::fixed << std::setprecision(2);
    //     std::cout << n.name << ", " << n.danceability << ", " << n.energy << ", " << n.loudness << ", " << n.valence << std::endl;
    // }

    int mood = 2;
    readEdgelist(g, mood);
    makeAdj(g);

    // Debug: Print edges
    // std::cout << "Edges:" << std::endl;
    // for (const auto &e : g.edges) {
    //     std::cout << g.nodes[e.source].name << " - " << g.nodes[e.target].name << std::endl;
    // }

    std::unordered_set<int> R, P, X;
    for (unsigned i = 0; i < g.n(); i++) {
        P.insert(i);
    }

    std::vector<std::unordered_set<int>> cliques;
    bronKerboschPivot(g, R, P, X, cliques);
    std::set<int> tracks;
    for(auto &x: cliques)
    {
        for(auto &y: x)
        {
            tracks.insert(y);
        }
    }
    std::vector<int> tracks_vec(tracks.begin(), tracks.end());
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::cout << "Here are your recommended songs..." << std::endl;
    for(unsigned i = 0; i < 5; i++)
    {
        std::uniform_int_distribution<int> dis(0, g.n()-1);
        int index = dis(gen);
        std::vector<int> prev(5);
        prev.push_back(index);
        // for(auto &x: prev)
        // {
        //     if(index == x)
        //     {
        //         index = dis(gen);
        //     }
        // }
        std::cout << g.nodes[tracks_vec[index]].name << "-" << g.nodes[tracks_vec[index]].artist << std::endl;
    }

    return 0;
}
