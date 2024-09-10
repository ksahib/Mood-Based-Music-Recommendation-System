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
#include <conio.h>

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
    std::vector<unsigned> lab; // label of nodes
    std::vector<std::vector<unsigned>> sub; // sub[l]: nodes in G_l
    unsigned n() const {
        return nodes.size(); // no. of nodes
    }
    unsigned e() const {
        return edges.size(); // no. of edges
    }
} graph;

//builds the nodes of the graph from the dataset
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

//creates edges based on the mood parameter
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

//generates the adjacency list of the graph
void makeAdj(graph &g) {
    unsigned md = 0; // max degree
    std::vector<unsigned> d0(g.n(), 0);
    //calculates degree
    for (auto &e : g.edges) {
        d0[e.source]++;
        d0[e.target]++;
    }
    for (auto &x : d0) {
        md = std::max(x, md);
    }
    //initialisation for bin sort
    std::vector<unsigned> bin(md + 1, 0);
    std::vector<unsigned> pos(g.n());
    std::vector<unsigned> vert(g.n());
    //calculates the frequency of the degrees
    for (unsigned i = 0; i < g.n(); i++) {
        bin[d0[i]]++;
    }
    //calculates the starting position of the degrees
    unsigned start = 0;
    for (unsigned i = 0; i <= md; i++) {
        unsigned num = bin[i];
        bin[i] = start;
        start += num;
    }
    //calculates positions of the nodes in the vert array
    for (unsigned i = 0; i < g.n(); i++) {
        pos[i] = bin[d0[i]];
        vert[pos[i]] = i; // contains the nodes in increasing order of degree
        bin[d0[i]]++;
    }
    g.d = vert;
    std::vector<unsigned> cd0(g.n() + 1, 0); 
    std::vector<unsigned> adj0(2 * g.e(), 0);
    g.adj.resize(2 * g.e());
    for (unsigned i = 1; i < g.n() + 1; i++) {
        cd0[i] = cd0[i - 1] + d0[i - 1]; //cumulative degree
        d0[i - 1] = 0; //sets degrees to 0
    }
    //calculates adjacency: stores them in a way where cd stores the starting position and degree determines the number of spaces it has reserved for its neighbors
    for (unsigned i = 0; i < g.e(); i++) {
        adj0[cd0[g.edges[i].source] + d0[g.edges[i].source]++] = g.edges[i].target;
        adj0[cd0[g.edges[i].target] + d0[g.edges[i].target]++] = g.edges[i].source;
    }
    g.adj = adj0;
    g.cd = cd0;
}

//returns a set of neighbors
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

//implementation of the bron kerbosch pivot algorithm. Finds the maximal cliques in a graph
void bronKerboschPivot(const graph& g, 
                       std::unordered_set<int>& R, 
                       std::unordered_set<int>& P, 
                       std::unordered_set<int>& X, 
                       std::vector<std::unordered_set<int>>& cliques) {
    //base case: reports cliques when both P and X are empty
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
                pivot = g.d[i]; // chooses the highest degree node as the pivot
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
                              std::inserter(P_inter_Nv, P_inter_Nv.begin())); // P intersection N(v)
        std::set_intersection(X.begin(), X.end(), N_v.begin(), N_v.end(),
                              std::inserter(X_inter_Nv, X_inter_Nv.begin())); // X intersection N(v)

        bronKerboschPivot(g, R, P_inter_Nv, X_inter_Nv, cliques); // recursive call

        R.erase(v); // erase v from the candidate set
        P.erase(v); //erase v from the unexplored set
        X.insert(v); // insert v to the explored set
    }
}

bool fileExists(const std::string &filename)
{
    std::ifstream file(filename);
    return file.good();
}

void addToPlaylist(graph &g, int i)
{
    std::string filename = "playlist.txt";
    std::ofstream file;
    file.open(filename, std::ios_base::app);
    file << g.nodes[i].name << "-" << g.nodes[i].artist << std::endl;

}

void loadPlaylist()
{
    std::ifstream file("playlist.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
}

void loadLogo()
{
    std::string logo =  R"(
                                                                                                    
       ,--.                                                                                         
   ,--/  /|              .--.--.        ___                                                  ____   
,---,': / '             /  /    '.    ,--.'|_                                              ,'  , `. 
:   : '/ /      ,---,. |  :  /`. /    |  | :,'    __  ,-.                               ,-+-,.' _ | 
|   '   ,     ,'  .' | ;  |  |--`     :  : ' :  ,' ,'/ /|                            ,-+-. ;   , || 
'   |  /    ,---.'   , |  :  ;_     .;__,'  /   '  | |' |    ,---.      ,--.--.     ,--.'|'   |  || 
|   ;  ;    |   |    |  \  \    `.  |  |   |    |  |   ,'   /     \    /       \   |   |  ,', |  |, 
:   '   \   :   :  .'    `----.   \ :__,'| :    '  :  /    /    /  |  .--.  .-. |  |   | /  | |--'  
|   |    '  :   |.'      __ \  \  |   '  : |__  |  | '    .    ' / |   \__\/: . .  |   : |  | ,     
'   : |.  \ `---'       /  /`--'  /   |  | '.'| ;  : |    '   ;   /|   ," .--.; |  |   : |  |/      
|   | '_\.'            '--'.     /    ;  :    ; |  , ;    '   |  / |  /  /  ,.  |  |   | |`-'       
'   : |                  `--'---'     |  ,   /   ---'     |   :    | ;  :   .'   \ |   ;/           
;   |,'                                ---`-'              \   \  /  |  ,     .-./ '---'            
'---'                                                       `----'    `--`---'                      
                                                                                                    
)";
    std::cout << "\t\t\t\t" << logo << std::endl;
}

// main UI
int home(graph &g, std::vector<int> &prev)
{
    system("cls");
    loadLogo();
    std::cout << "Your playlist: " << std::endl;
    loadPlaylist();
    std::cout << "\t\t\t\tPick your color" <<std::endl;
    std::cout << "\t\t\t\t0.Red 1.Yellow 2.Cyan 3.Blue 4.Black" <<std::endl;
    int mood;
    std::cin >> mood;
    readEdgelist(g, mood);
    makeAdj(g);
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
    std::cout << "Songs we recommend...." << std::endl;
    prev.clear();
    for(unsigned i = 0; i < 5; i++)
    {
        std::uniform_int_distribution<int> dis(0, g.n()-1);
        int index = dis(gen);
        prev.push_back(index);
        std::cout << i << " " << g.nodes[tracks_vec[index]].name << "-" << g.nodes[tracks_vec[index]].artist << std::endl;
    }
    std::cout << "5. Home" << std::endl;
    std::cout << "6.exit" << std::endl;
    return mood;

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
        
        
    }
    graph g;
    buildGraph(g);
    std::vector<int>prev;
    home(g, prev);

    

    // Debug: Print nodes
    // std::cout << "Nodes:" << std::endl;
    // for (const auto &n : g.nodes) {
    //     std::cout << std::fixed << std::setprecision(2);
    //     std::cout << n.name << ", " << n.danceability << ", " << n.energy << ", " << n.loudness << ", " << n.valence << std::endl;
    // }

    // system("cls");
    // loadLogo();
    

    // Debug: Print edges
    // std::cout << "Edges:" << std::endl;
    // for (const auto &e : g.edges) {
    //     std::cout << g.nodes[e.source].name << " - " << g.nodes[e.target].name << std::endl;
    // }


    int trackid;
    std::cout << "choose the songs you want to add to your playlist" << std::endl;
    while(trackid != 6)
    {
        std::cin >> trackid;
        if(trackid == 5)
        {
            //system("cls");
            home(g, prev);
        }
        else
        {
            addToPlaylist(g, prev[trackid]);
        }
        
    }

    return 0;
}
