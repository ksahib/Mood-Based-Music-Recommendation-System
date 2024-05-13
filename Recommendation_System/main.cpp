#include<iostream>
#include<vector>
#include<fstream>
#include<sstream>
#include<string>

using namespace std;

typedef struct
{
    string name;
    double danceability;
    double energy;
    double loudness;
    double valence;
    
}node;

typedef struct
{
    unsigned source;
    unsigned target;
}edge;

typedef struct
{
  std::vector<edge> edges; // list of edges
  std::vector<node> nodes; // list of nodes
  std::vector<unsigned> ns;             // ns[l]: number of nodes in G_l
  std::vector<std::vector<unsigned>> d; // d[l]: degrees of G_l
  std::vector<unsigned> cd;             // cumulative degree: (starts with 0) length=n+1
  std::vector<unsigned> adj;            // truncated list of neighbors
  std::vector<unsigned> lab;              // lab[i] label of node i
  std::vector<std::vector<unsigned>> sub; // sub[l]: nodes in G_l
  unsigned n = nodes.size(); // number of nodes
  unsigned e = edges.size(); // number of edges

} graph;

void buildGraph(graph g)
{
    node n;
    ifstream file("playlist_audio_features.txt");
    if (!file.is_open())
    {
        std::cerr << "unable to open file" << std::endl;
    }
    string line;
    while(std::getline(file, line))
    {
        istringstream iss(line);
        string name, danceability, energy, loudness, valence;
        if(iss >> name >> danceability >> energy >> loudness >> valence)
        {
            n.name = name;
            n.danceability = stod(danceability);
            n.energy = stod(energy);
            n.loudness = stod(loudness);
            n.valence = stod(valence);
        }
        g.nodes.push_back(n);

    }
    file.close();
}

void createEdgelist()
{
    
}