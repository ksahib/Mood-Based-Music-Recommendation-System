#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <set>
#include <iterator>
#include <iomanip>
#include <locale>

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
                std::cerr << "Invalid argument: " << e.what() << " in line: " << line << std::endl;
            } catch (const std::out_of_range &e) {
                std::cerr << "Out of range: " << e.what() << " in line: " << line << std::endl;
            }
        }
    }
    file.close();
}

void createEdgelist(graph &g, int mood) {
    std::string filename = "graph" + std::to_string(mood) + ".txt";
    std::ofstream file;
    file.open(filename);
    for (unsigned i = 0; i < g.n(); i++) {
        const node &node_i = g.nodes[i];
        bool condition = false;
        switch (mood) {
            case 0:
                condition = (node_i.energy >= 0.8 && node_i.loudness >= -7.0 && node_i.valence <= 0.4);
                break;
            case 1:
                condition = (node_i.danceability >= 0.5 && node_i.energy >= 0.5 && node_i.valence >= 0.7);
                break;
            case 2:
                condition = (node_i.danceability >= 0.4 && node_i.energy >= 0.4 && node_i.valence >= 0.3 && node_i.valence < 0.7);
                break;
            case 3:
                condition = (node_i.danceability <= 0.4 && node_i.energy <= 0.4 && node_i.valence <= 0.2);
                break;
            case 4:
                condition = (node_i.danceability <= 0.3 && node_i.valence <= 0.1);
                break;
        }
        if (condition) {
            for (unsigned j = i + 1; j < g.n(); j++) {
                const node &node_j = g.nodes[j];
                bool condition_j = false;
                switch (mood) {
                    case 0:
                        condition_j = (node_j.energy >= 0.8 && node_j.loudness >= -7.0 && node_j.valence <= 0.4);
                        break;
                    case 1:
                        condition_j = (node_j.danceability >= 0.5 && node_j.energy >= 0.5 && node_j.valence >= 0.7);
                        break;
                    case 2:
                        condition_j = (node_j.danceability >= 0.4 && node_j.energy >= 0.4 && node_j.valence >= 0.3 && node_j.valence < 0.7);
                        break;
                    case 3:
                        condition_j = (node_j.danceability <= 0.4 && node_j.energy <= 0.4 && node_j.valence <= 0.2);
                        break;
                    case 4:
                        condition_j = (node_j.danceability <= 0.3 && node_j.valence <= 0.1);
                        break;
                }
                if (condition_j) {
                    file << i << " " << j << std::endl;
                }
            }
        }
    }
    file.close();
}

int main(int argc, char **argv)
{
    graph g;
    buildGraph(g);
    unsigned mood = atoi(argv[1]);
    createEdgelist(g,mood);
}